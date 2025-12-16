// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Chess/EPSkill_CRook.h"
#include "Kismet/GameplayStatics.h"
#include "Data/EPSkillDataAsset.h"
#include "Characters/EPCombatCharacterBase.h"
#include "GenericTeamAgentInterface.h"  
#include "Kismet/KismetSystemLibrary.h"
#include "AI/ChessUnitController.h"

void UEPSkill_CRook::Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	ActorsToIgnore.Empty();
	ActorsToIgnore.Add(Caster);
	for (int32 i = 0;i < 4;i++) IsValidXY[i] = true;
	Explode(Caster, 1);
}

void UEPSkill_CRook::Explode(AActor* Caster, int32 Count)
{
	AEPCombatCharacterBase* Character = Cast<AEPCombatCharacterBase>(Caster);
	if (!Character || !SkillDataAsset) return;

	// 스킬 데이터에서 첫 번째 공격 단계의 정보를 가져옵니다.
	// 자폭 스킬은 보통 단일 단계이므로 0번 인덱스를 사용합니다.
	const FEPSkillPhaseData* PhaseData = GetPhaseData(0);
	if (!PhaseData) return;


	// 폭발 위치 지정 (사용자 위치)
	const FVector ExplosionLocation = Caster->GetActorLocation();

	// 스킬 데이터의 피해량, 범위, 이펙트, 사운드 정보를 가져옴
	const float Damage = PhaseData->Damage;
	TargetActors.Empty();

	if (PhaseData->TargetType != EEPTargetType::Self)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target type does not match --> skill misfire"));
		return;
	}

	const EEPSkillRangeShape RangeType = PhaseData->SkillRange.ShapeType;
	float TempRadius = 0.0f;
	if (RangeType != EEPSkillRangeShape::Sphere)
	{
		TempRadius = 100.0f; // 임시 값 사용
	}
	else
	{
		TempRadius = PhaseData->SkillRange.Dimensions.X; // Sphere의 X를 반지름으로 사용
	}
	const float DamageRadius = TempRadius;

	UNiagaraSystem* Effect = PhaseData->VFX.Get(); // 소프트 포인터에서 실제 애셋 가져오기
	USoundBase* Sound = PhaseData->SFX.Get();

#if !(UE_BUILD_SHIPPING)
	// 디버그용
	AChessUnitController* DebugController = Cast<AChessUnitController>(Character->GetController());
	FIntPoint DebugXY = DebugController->GetXY();
	for (int32 i = 0;i < 4;i++) {
		FIntPoint DebugTempXY = DebugXY;
		DebugTempXY.X += TempXY[i].X * Count;
		DebugTempXY.Y += TempXY[i].Y * Count;
		if (DebugController->IsOnBoard(DebugTempXY)) {
			FVector NewVec = DebugController->GetGridVector(DebugTempXY);
			DrawDebugSphere(Caster->GetWorld(), NewVec, DamageRadius, 24, FColor::Red, false, 2.f, 0, 2.f);
		}
	}
#endif
	AChessUnitController* Controller = Cast<AChessUnitController>(Character->GetController());
	for (int32 i = 0;i < 4;i++) {
		FIntPoint NewXY = Controller->GetXY();
		NewXY.X += TempXY[i].X * Count;
		NewXY.Y += TempXY[i].Y * Count;
		if (Controller->IsOnBoard(NewXY)) Controller->SetGridWarning(NewXY, -4);
		else IsValidXY[i] = false;
	}

	// 시전자의 팀 ID를 가져옴
	FGenericTeamId CasterTeamId = FGenericTeamId::NoTeam; // 기본값 = '팀 없음'
	if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Caster))
	{
		CasterTeamId = TeamAgent->GetGenericTeamId();
	}

	for (int32 i = 0;i < 4;i++) {
		if (IsValidXY[i]) {
			// 폭발 범위 내의 모든 캐릭터를 검색
			TArray<AActor*> OverlappedActors;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); // Pawn 타입만 검색

			FIntPoint NewXY = Controller->GetXY();
			NewXY.X += TempXY[i].X * Count;
			NewXY.Y += TempXY[i].Y * Count;
			FVector NewVec = Controller->GetGridVector(NewXY);
			UKismetSystemLibrary::SphereOverlapActors(
				Caster->GetWorld(),
				NewVec,
				DamageRadius,
				ObjectTypes,
				ACharacter::StaticClass(),
				TArray<AActor*>(),
				OverlappedActors
			);

			// 찾은 액터들을 순회하며 같은 팀인지 확인
			if (CasterTeamId != FGenericTeamId::NoTeam) // 시전자가 팀이 있을 경우에만 검사
			{
				for (AActor* OverlappedActor : OverlappedActors)
				{
					IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(OverlappedActor);
					// 대상이 같은 팀이라면, 무시 목록에 추가합니다.
					if (TargetTeamAgent && TargetTeamAgent->GetGenericTeamId() == CasterTeamId)
					{
						ActorsToIgnore.Add(OverlappedActor);
					}
					else if (!ActorsToIgnore.Contains(OverlappedActor))
					{
						ActorsToIgnore.Add(OverlappedActor);
						TargetActors.Add(OverlappedActor);
					}
				}
			}
		}
	}
	// -----------------------------------------------

	// 해당 위치에 광역 피해(Radial Damage)를 입힘
	for (AActor* TargetActor : TargetActors) {
		UGameplayStatics::ApplyDamage(
			TargetActor,
			Damage,
			Character->GetController(),
			Caster,
			UDamageType::StaticClass()
		);
	}

	// 폭발 이펙트 및 사운드 재생
	if (Effect)
	{
		//UGameplayStatics::SpawnEmitterAtLocation(Caster->GetWorld(), Effect, ExplosionLocation);
	}
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(Caster->GetWorld(), Sound, ExplosionLocation);
	}

	bool NextOK = false;
	for (int32 i = 0;i < 4;i++) {
		if (IsValidXY[i] == true) NextOK = true;
	}
	if (NextOK) {
		FTimerHandle TimerHandle;
		Caster->GetWorldTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([=, this]() {
				if (IsValid(Caster))
				{
					Explode(Caster, Count + 1);
				}
				}),
			0.2f,
			false
		);
	}
}
