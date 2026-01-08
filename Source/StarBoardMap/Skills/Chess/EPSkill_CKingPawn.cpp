// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Chess/EPSkill_CKingPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Data/EPSkillDataAsset.h"
#include "Characters/EPCombatCharacterBase.h"
#include "GenericTeamAgentInterface.h"  
#include "Kismet/KismetSystemLibrary.h"  
#include "AI/ChessUnitController.h"
#include "NiagaraFunctionLibrary.h"

void UEPSkill_CKingPawn::Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	// 시전자가 유효한지, 스킬 데이터가 할당되었는지 확인
	AEPCombatCharacterBase* Character = Cast<AEPCombatCharacterBase>(Caster);
	if (!Character || !SkillDataAsset) return;

	// 스킬 데이터에서 첫 번째 공격 단계의 정보를 가져옵니다.
	// 자폭 스킬은 보통 단일 단계이므로 0번 인덱스를 사용합니다.
	const FEPSkillPhaseData* PhaseData = GetPhaseData(0);
	if (!PhaseData) return;

	UNiagaraSystem* Effect = PhaseData->VFX.Get(); // 소프트 포인터에서 실제 애셋 가져오기
	USoundBase* Sound = PhaseData->SFX.Get();

	// 폭발 위치 지정 (사용자 위치)
	const FVector ExplosionLocation = Caster->GetActorLocation();

	// 스킬 데이터의 피해량, 범위, 이펙트, 사운드 정보를 가져옴
	const float Damage = PhaseData->Damage;

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

	FIntPoint TempXY[8] = {
		FIntPoint(0,150),
		FIntPoint(0, -150),
		FIntPoint(150, 150),
		FIntPoint(150, 0),
		FIntPoint(150,-150),
		FIntPoint(-150, -150),
		FIntPoint(-150,0),
		FIntPoint(-150,150) };

	AChessUnitController* Controller = Cast<AChessUnitController>(Character->GetController());
	for (int32 i = 0;i < 8;i++) {
		FVector NewVec = ExplosionLocation;
		NewVec.X += TempXY[i].X;
		NewVec.Y += TempXY[i].Y;

		FIntPoint NewXY = Controller->GetXY();
		NewXY.X += TempXY[i].X / 150;
		NewXY.Y += TempXY[i].Y / 150;
		if (Controller->IsOnBoard(NewXY)) {
			Controller->SetGridWarning(NewXY, -Controller->GetUnitType());

			// 디버그용
			//DrawDebugSphere(Caster->GetWorld(), NewVec, DamageRadius, 24, FColor::Red, false, 2.f, 0, 2.f);
			
			//나이아가라
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(Caster->GetWorld(), Effect, NewVec);
			UGameplayStatics::PlaySoundAtLocation(Caster->GetWorld(), Sound, NewVec);
		}
	}

	// ------------ 팀 식별 로직 시작 ------------
	// 피해를 무시할 액터들을 담을 배열 생성
	TSet<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Caster); // 시전자 자신은 항상 무시 목록에 포함
	TSet<AActor*> TargetActors;

	// 시전자의 팀 ID를 가져옴
	FGenericTeamId CasterTeamId = FGenericTeamId::NoTeam; // 기본값 = '팀 없음'
	if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Caster))
	{
		CasterTeamId = TeamAgent->GetGenericTeamId();
	}

	for (int32 i = 0;i < 8;i++) {
		// 폭발 범위 내의 모든 캐릭터를 검색
		TArray<AActor*> OverlappedActors;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); // Pawn 타입만 검색

		FIntPoint NewXY = Controller->GetXY();
		NewXY.X += TempXY[i].X / 150;
		NewXY.Y += TempXY[i].Y / 150;
		if (Controller->IsOnBoard(NewXY)) {

			FVector NewVec = ExplosionLocation;
			NewVec.X += TempXY[i].X;
			NewVec.Y += TempXY[i].Y;
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

	//데미지
	for (AActor* TargetActor : TargetActors) {
		UGameplayStatics::ApplyDamage(
			TargetActor,
			Damage,
			Character->GetController(),
			Caster,
			UDamageType::StaticClass()
		);
	}
}

