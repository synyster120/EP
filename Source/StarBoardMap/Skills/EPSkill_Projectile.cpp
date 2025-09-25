// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkill_Projectile.h"
#include "Skills/EPProjectileBase.h"
#include "Core/Subsystems/EPObjectPoolManager.h"
#include "Data/EPSkillDataAsset.h"
#include "GameFramework/Character.h"
#include "Data/EPObjectPoolTypes.h"
#include "Core/Helper/EPAsyncLoadHelper.h"
#include "Characters/EPCombatCharacterBase.h"

void UEPSkill_Projectile::Activate(ACharacter* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	Super::Activate(Caster, NewTargetData, CurrentComboIndex);

    UE_LOG(LogTemp, Warning, TEXT("EP_Error:: UEPSkill_Projectile -- Activate"));

    // 유효 확인
    if (!Caster || !SkillDataAsset) return;

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

            // 투사체가 생성될 위치와 방향(Transform)을 계산
            //    (예: 캐릭터의 특정 소켓 위치, 타겟 방향 등)

            if (NewTargetData.TargetType == EEPTargetType::Direction)
            {
                // -------------- 생성 위치 계산 --------------
                const FVector CasterLocation = Caster->GetActorLocation();
                const FRotator CasterRotation = Caster->GetActorRotation();

                //  캐릭터가 바라보는 정면 방향 벡터
                const FVector ForwardVector = Caster->GetActorForwardVector();
                const float Distance = 50.0f; // 원하는 거리

                // 현재 위치에서 정면 방향으로 원하는 거리만큼 떨어진 위치 계산
                const FVector SpawnLocation = CasterLocation + (ForwardVector * Distance);

                // 최종적으로 생성될 위치(Location)와 방향(Rotation)을 FTransform으로 만듦
                const FTransform SpawnTransform(CasterRotation, SpawnLocation);


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

                    // 풀 매니저에게 투사체 스폰을 요청
                    PoolManager->SpawnObjectFromPool(ActorClassToSpawn, SpawnTransform, InitializerData);
                }

                // -------------- 애니메이션 재생 요청 --------------
                AEPCombatCharacterBase* Character = Cast<AEPCombatCharacterBase>(Caster);
                if (!Character) return;

                Character->PlayAnimationByTag(PhaseData.AnimationTag);

                UE_LOG(LogTemp, Warning, TEXT("Skill_melee combo --> playing animation : %s"), *PhaseData.AnimationTag.ToString());
            }
        });

}
