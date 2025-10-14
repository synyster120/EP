// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkill_ExplodeSelf.h"
#include "Kismet/GameplayStatics.h"
#include "Data/EPSkillDataAsset.h"
#include "AI/EPEnemyAIController.h"
#include "Characters/EPCombatCharacterBase.h"
#include "Components/EPSkillComponent.h"


void UEPSkill_ExplodeSelf::Activate(ACharacter* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	// 부모의 Activate를 호출하여 공통 로직(쿨타임 시작 등)을 처리할 수 있습니다.
	// Super::Activate(Caster, TargetData);

	// 시전자가 유효한지, 스킬 데이터가 할당되었는지 확인
	AEPCombatCharacterBase* Character = Cast<AEPCombatCharacterBase>(Caster);
	if (!Character || !SkillDataAsset) return;

	// 스킬 데이터에서 첫 번째 공격 단계의 정보를 가져옵니다.
	// 자폭 스킬은 보통 단일 단계이므로 0번 인덱스를 사용합니다.
	const FEPSkillPhaseData* PhaseData = GetPhaseData(0);
	if (!PhaseData) return;

	// --- 자폭 로직 시작 ---

	// Enemy일 경우, Blackboard 의 대기 상태 true 로 업데이트
	if (AEPEnemyAIController* EnemyAIController = Cast<AEPEnemyAIController>(Caster->GetController()))
	{
		EnemyAIController->NotifyIsWindupUpdate();
	}


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

	UNiagaraSystem* Effect = PhaseData->VFX.Get(); // 소프트 포인터에서 실제 애셋 가져오기
	USoundBase* Sound = PhaseData->SFX.Get();

#if !(UE_BUILD_SHIPPING)
	// 디버그용
	DrawDebugSphere(Caster->GetWorld(), Caster->GetActorLocation(), DamageRadius, 24, FColor::Red, false, 2.f, 0, 2.f);
#endif

	// Enemy일 경우, Blackboard 의 대기 상태 false 로 업데이트
	if (AEPEnemyAIController* EnemyAIController = Cast<AEPEnemyAIController>(Caster->GetController()))
	{
		EnemyAIController->NotifyIsWindupUpdate();
	}

	// 해당 위치에 광역 피해(Radial Damage)를 입힘
	UGameplayStatics::ApplyRadialDamage(
		Caster->GetWorld(),
		Damage,
		ExplosionLocation,
		DamageRadius,
		UDamageType::StaticClass(),
		TArray<AActor*>(), // 피해를 무시할 액터 목록 (필요하다면)
		Caster, // 피해를 입힌 가해자
		Caster->GetController() // 가해자의 컨트롤러
	);

	// 폭발 이펙트 및 사운드 재생
	if (Effect)
	{
		//UGameplayStatics::SpawnEmitterAtLocation(Caster->GetWorld(), Effect, ExplosionLocation);
	}
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(Caster->GetWorld(), Sound, ExplosionLocation);
	}

	// 시전자 자신 파괴
	Caster->Destroy();
}

