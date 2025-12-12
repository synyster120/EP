// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkill_Projectile.h"
#include "Skills/EPProjectileBase.h"
#include "Core/Subsystems/EPObjectPoolManager.h"
#include "Data/EPSkillDataAsset.h"
#include "GameFramework/Character.h"
#include "Data/EPObjectPoolTypes.h"
#include "Core/Helper/EPAsyncLoadHelper.h"
#include "Characters/EPCombatCharacterBase.h"
#include "Kismet/GameplayStatics.h"

// 테스트용
#include "GameFramework/ProjectileMovementComponent.h"

void UEPSkill_Projectile::Activate(ACharacter* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	Super::Activate(Caster, NewTargetData, CurrentComboIndex);

    if (!Caster || !SkillDataAsset ) return;

    const FEPSkillData& Data = SkillDataAsset->SkillData;
    if (!Data.ComboSequence.IsValidIndex(0)) return;

    // 스킬 데이터에서 현재 콤보 단계에 맞는 데이터 가져옴 (콤보가 아니면 0)
    const FEPSkillPhaseData PhaseData = *GetPhaseData(0);

    // 투사체 클래스 비동기 로드 요청
    UEPAsyncLoadHelper::RequestAsyncLoad<AEPProjectileBase>(PhaseData.ProjectileClass,
        [this, NewTargetData, Caster, PhaseData](TSubclassOf<AEPProjectileBase> LoadedProjectileClass)
        {
            // 투사체 클래스 로드 정상 완료 확인
            if (!LoadedProjectileClass)
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to load SkillClass for %s"), *PhaseData.AnimationTag.ToString());
                return;
            }
            UE_LOG(LogTemp, Warning, TEXT("Success to load SkillClass for %s"), *PhaseData.AnimationTag.ToString());

            FTransform SpawnTransform; // 최종 목적 위치
            FVector LaunchVelocity = FVector::ZeroVector; // 최종 발사 속도를 담을 변수

            const FVector CasterLocation = Caster->GetActorLocation();
            const FRotator CasterRotation = Caster->GetActorRotation();

            // 투사체가 생성될 위치와 방향(Transform)을 계산
            //    (예: 캐릭터의 특정 소켓 위치, 타겟 방향 등)

            // 헬퍼 함수를 호출하여 모든 복잡한 계산을 위임
            if (CalculateLaunchVelocity(Caster, NewTargetData, PhaseData, SpawnTransform, LaunchVelocity))
            {

                // 풀 매니저에게 스폰을 요청
                if (UEPObjectPoolManager* PoolManager = Caster->GetGameInstance()->GetSubsystem<UEPObjectPoolManager>())
                {
                    // TSubclassOf<AEPProjectileBase>를 범용 TSoftClassPtr<AActor>로 변환 후 actor 반환 받음
                    TSoftClassPtr<AActor> ActorClassToSpawn = PhaseData.ProjectileClass;

                    // 스폰 요청 위한 데이터 세팅
                    FEPPoolableObjectInitializer InitializerData;
                    InitializerData.Owner = Caster;
                    InitializerData.Data = this->SkillDataAsset;
                    InitializerData.TargetData = NewTargetData;
                    InitializerData.LaunchVelocity = LaunchVelocity;

                    // 풀 매니저에게 투사체 스폰을 요청
                    AActor* SpawnedActor = PoolManager->SpawnObjectFromPool(ActorClassToSpawn, SpawnTransform, InitializerData);
                    //AEPProjectileBase* Projectile = Cast<AEPProjectileBase>(SpawnedActor);

                    //if (Projectile && Projectile->GetProjectileMovementComponent())
                    //{
                    //    // 최종적으로 계산된 속도를 투사체에 적용
                    //    Projectile->GetProjectileMovementComponent()->Velocity = LaunchVelocity;
                    //}
                }
            }


            // -------------- 투사체 요청 --------------
                // 풀 매니저를 가져옴
            if (UEPObjectPoolManager* PoolManager = Caster->GetGameInstance()->GetSubsystem<UEPObjectPoolManager>())
            {
                // TSubclassOf<AEPProjectileBase>를 범용 TSoftClassPtr<AActor>로 변환 후 actor 반환 받음
                TSoftClassPtr<AActor> ActorClassToSpawn = PhaseData.ProjectileClass;
                //AActor* SpawnedActor = PoolManager->GetObjectFromPool(ActorClassToSpawn);

                // 스폰 요청 위한 데이터 세팅
                FEPPoolableObjectInitializer InitializerData;
                InitializerData.Owner = Caster;
                InitializerData.Data = SkillDataAsset;
                InitializerData.TargetData = NewTargetData;

                // 풀 매니저에게 투사체 스폰을 요청
                PoolManager->SpawnObjectFromPool(ActorClassToSpawn, SpawnTransform, InitializerData);
            }

            // -------------- 애니메이션 재생 요청 --------------
            AEPCombatCharacterBase* Character = Cast<AEPCombatCharacterBase>(Caster);
            if (!Character) return;

            Character->PlayAnimationByTag(PhaseData.AnimationTag);
            UE_LOG(LogTemp, Warning, TEXT("Skill_projectile --> playing animation : %s"), *PhaseData.AnimationTag.ToString());

        });

}

bool UEPSkill_Projectile::CalculateLaunchVelocity(ACharacter* Caster, const FEPSkillTargetData& TargetData, const FEPSkillPhaseData& PhaseData, FTransform& OutSpawnTransform, FVector& OutLaunchVelocity) const
{
    const FVector CasterLocation = Caster->GetActorLocation();
    const FRotator CasterRotation = Caster->GetActorRotation();

    // -------------- 생성 위치 계산 --------------
    switch (TargetData.TargetType)
    {
    case EEPTargetType::Direction:
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] Projectile target type -- direction"), *Caster->GetName());
        const FVector ForwardVector = Caster->GetActorForwardVector();
        const float Distance = 50.0f;
        const FVector SpawnLocation = CasterLocation + (ForwardVector * Distance);
        OutSpawnTransform = FTransform(CasterRotation, SpawnLocation);
        OutLaunchVelocity = Caster->GetActorForwardVector() * PhaseData.ProjectileInfo.InitialSpeed;
        break;
    }
    case EEPTargetType::Actor:
    {
        if (TargetData.TargetActor)
        {
            FVector SuggestedVelocity;
            // 최적의 포물선 속도를 계산
            bool bSuccess = UGameplayStatics::SuggestProjectileVelocity(
                this,
                SuggestedVelocity,
                CasterLocation + 5.0f, // 시작 위치
                TargetData.TargetActor->GetActorLocation(), // 목표 위치
                1500.0f, // 속력
                false, 0.0f, 1.0f // 기타 옵션
            );

            if (bSuccess) // 커스텀 속도 계산에 성공했는지 여부
            {
                OutLaunchVelocity = SuggestedVelocity;
                // 발사 방향을 속도 방향과 일치
                OutSpawnTransform = FTransform(SuggestedVelocity.Rotation(), CasterLocation);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[%s] Projectile target type -- actor is not set"), *Caster->GetName());
        }
        break;
    }
    case EEPTargetType::Location:
    {
        // 타겟 위치를 향하는 방향으로 SpawnTransform 설정
        FVector DirectionToLocation = (TargetData.TargetLocation - CasterLocation).GetSafeNormal();
        OutSpawnTransform = FTransform(DirectionToLocation.Rotation(), CasterLocation);
        break;
    }
    default :
        // 어떤 경우에도 성공하지 못했다면 기본값(정면 발사)을 사용
        OutSpawnTransform = FTransform(CasterRotation, CasterLocation);
        OutLaunchVelocity = CasterRotation.Vector() * PhaseData.ProjectileInfo.InitialSpeed;
    }

    return true;
}
