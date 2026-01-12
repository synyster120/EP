// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkill_ExplodeSelf.h"
#include "Kismet/GameplayStatics.h"
#include "Data/EPSkillDataAsset.h"
#include "AI/EPEnemyAIController.h"
#include "Characters/EPCombatCharacterBase.h"
#include "Components/EPSkillComponent.h"

#include "GenericTeamAgentInterface.h"       // IGenericTeamAgentInterface를 사용하기 위해
#include "Kismet/KismetSystemLibrary.h"      // SphereOverlapActors를 사용하기 위해

#include "Core/Helper/EPAsyncLoadHelper.h"
#include "Components/EPMovementLockComponent.h"
#include "Data/EPCharacterAnimationData.h"


void UEPSkill_ExplodeSelf::Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	// 시전자가 유효한지, 스킬 데이터가 할당되었는지 확인
	AEPCombatCharacterBase* Character = Cast<AEPCombatCharacterBase>(Caster);
	if (!Character || !SkillDataAsset) return;

	// 스킬 데이터에서 첫 번째 공격 단계의 정보를 가져옵니다.
	// 자폭 스킬은 보통 단일 단계이므로 0번 인덱스를 사용합니다.
	const FEPSkillPhaseData* PhaseData = GetPhaseData(0);
	if (!PhaseData) return;

	// Enemy일 경우, Blackboard 의 대기 상태 true 로 업데이트
	if (AEPEnemyAIController* EnemyAIController = Cast<AEPEnemyAIController>(Character->GetController()))
	{
		//EnemyAIController->NotifyIsWindupUpdate();
	}

	// 폭발 위치 지정 (사용자 위치)
	ExplosionLocation = Caster->GetActorLocation();

	// 스킬 데이터의 피해량, 범위, 이펙트, 사운드 정보를 가져옴 ----
	Damage = PhaseData->Damage;

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
	DamageRadius = TempRadius;

	Effect = PhaseData->VFX.Get(); // 소프트 포인터에서 실제 애셋 가져오기
	Sound = PhaseData->SFX.Get();
	// --------------------

	// 움직임 제어 (Lock)
	OwnerCaster = Cast<AEPCharacterBase>(Caster);
	if (UEPMovementLockComponent* Lock = OwnerCaster->GetMovementLockComponent()) 
	{
		// Lock 설정
		Lock->Acquire(Locktext);
	}

	// 몽타주 비동기 로드 및 재생 요청
	OwnerCaster->PlayAnimationByTag(PhaseData->AnimationTag);

	// 원래라면 몽타주 "시작"타이밍에 맞춰서 타이머 시작해야함
	GetWorld()->GetTimerManager().SetTimer(
		SkillTimerHandle,
		this,
		&UEPSkill_ExplodeSelf::OnSkillEffectFinished,
		GetWindupSeconds(CurrentComboIndex),
		false
	);

	// 폭발 이펙트 및 사운드 재생
	if (Effect)
	{
		//UGameplayStatics::SpawnEmitterAtLocation(Caster->GetWorld(), Effect, ExplosionLocation);
	}
	if (Sound)
	{
		//UGameplayStatics::PlaySoundAtLocation(Caster->GetWorld(), Sound, ExplosionLocation);
	}
}

// 정상적 종료
void UEPSkill_ExplodeSelf::OnSkillEffectFinished()
{
	if (!Damage || !DamageRadius || ExplosionLocation.IsZero())
	{
		return;
	}

	// 폭발 이펙트 및 사운드 재생
	if (Effect)
	{
		//UGameplayStatics::SpawnEmitterAtLocation(Caster->GetWorld(), Effect, ExplosionLocation);
	}
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(OwnerCaster->GetWorld(), Sound, ExplosionLocation);
	}

#if !(UE_BUILD_SHIPPING)
	// 디버그용
	DrawDebugSphere(OwnerCaster->GetWorld(), OwnerCaster->GetActorLocation(), DamageRadius, 24, FColor::Red, false, 2.f, 0, 2.f);
#endif


	// ------------ 팀 식별 로직 시작 ------------
	// 피해를 무시할 액터들을 담을 배열 생성
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerCaster); // 시전자 자신은 항상 무시 목록에 포함

	// 시전자의 팀 ID를 가져옴
	FGenericTeamId CasterTeamId = FGenericTeamId::NoTeam; // 기본값 = '팀 없음'
	if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OwnerCaster))
	{
		CasterTeamId = TeamAgent->GetGenericTeamId();
	}

	// 폭발 범위 내의 모든 캐릭터를 검색
	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); // Pawn 타입만 검색

	UKismetSystemLibrary::SphereOverlapActors(
		OwnerCaster->GetWorld(),
		ExplosionLocation,
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
		}
	}

	// 해당 위치에 광역 피해(Radial Damage)를 입힘
	UGameplayStatics::ApplyRadialDamage(
		OwnerCaster->GetWorld(),
		Damage,
		ExplosionLocation,
		DamageRadius,
		UDamageType::StaticClass(),
		ActorsToIgnore, // 피해를 무시할 액터 목록
		OwnerCaster, // 피해를 입힌 가해자
		OwnerCaster->GetController() // 가해자의 컨트롤러
	);

	// Lock 해제
	if (UEPMovementLockComponent* Lock = OwnerCaster->GetMovementLockComponent())
	{
		Lock->Release(Locktext);
	}

	// 시전자 자신 파괴
	OwnerCaster->Destroy();
}

// 스킬 강제 중단
void UEPSkill_ExplodeSelf::CancelSkillActivation()
{
	// 타이머 정지
	GetWorld()->GetTimerManager().ClearTimer(SkillTimerHandle);

	// Lock 해제
	if (UEPMovementLockComponent* Lock = OwnerCaster->GetMovementLockComponent())
	{
		Lock->Release(Locktext);
	}
}

