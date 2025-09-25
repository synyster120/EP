// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkill_Projectile.h"
#include "Skills/EPProjectileBase.h"
#include "Core/Subsystems/EPObjectPoolManager.h"
#include "Data/EPSkillDataAsset.h"
#include "GameFramework/Character.h"

void UEPSkill_Projectile::Activate(ACharacter* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	Super::Activate(Caster, NewTargetData, CurrentComboIndex);

    // 필요한 데이터가 유효한지 확인합니다.
    if (!Caster || !SkillDataAsset) return;

    // 스킬 데이터에서 현재 콤보 단계에 맞는 데이터를 가져옵니다. (콤보가 아니면 0)
    const FEPSkillData& Data = SkillDataAsset->SkillData;
    if (!Data.ComboSequence.IsValidIndex(0)) return;
    const FEPSkillPhaseData& PhaseData = *GetPhaseData(0);

    // 스폰할 투사체 클래스가 유효한지 확인
    if (PhaseData.ProjectileClass)
    {
        // 투사체가 생성될 위치와 방향(Transform)을 계산
        //    (예: 캐릭터의 특정 소켓 위치, 타겟 방향 등)

        if (NewTargetData.TargetType == EEPTargetType::Direction)
        {
            const FVector CasterLocation = Caster->GetActorLocation();
            const FRotator CasterRotation = Caster->GetActorRotation();

            //  캐릭터가 바라보는 정면 방향 벡터
            const FVector ForwardVector = Caster->GetActorForwardVector();
            const float Distance = 50.0f; // 원하는 거리

            // 현재 위치에서 정면 방향으로 원하는 거리만큼 떨어진 위치 계산
            const FVector SpawnLocation = CasterLocation + (ForwardVector * Distance);

            // 최종적으로 생성될 위치(Location)와 방향(Rotation)을 FTransform으로 만듦
            const FTransform SpawnTransform(CasterRotation, SpawnLocation);
        }


        // -------------- 수정 필요 --------------


        //TSoftClassPtr<AActor> ActorClassToSpawn = PhaseData.ProjectileClass;
        // 풀 매니저를 가져옴
        if (UEPObjectPoolManager* PoolManager = Caster->GetGameInstance()->GetSubsystem<UEPObjectPoolManager>())
        {

            // 1. TSubclassOf<AEPProjectileBase>를 범용 TSoftClassPtr<AActor>로 변환합니다.
            //    이 변환은 안전합니다.
            TSoftClassPtr<AActor> ActorClassToSpawn = PhaseData.ProjectileClass;

            // 2. 이제 GetObjectFromPool 함수에 올바른 타입의 파라미터를 전달할 수 있습니다.
            AActor* SpawnedActor = PoolManager->GetObjectFromPool(ActorClassToSpawn);

            // 3. 반환된 범용 AActor*를, 우리가 실제로 필요한 AEPProjectileBase*로 '안전하게' 형 변환(Cast)합니다.
            AEPProjectileBase* Projectile = Cast<AEPProjectileBase>(SpawnedActor);

            if (Projectile)
            {
                // 이제 안전하게 투사체를 사용할 수 있습니다.
                /*Projectile->Initialize(...);
                Projectile->Activate(...);*/
            }



            // 풀 매니저에게 투사체 스폰을 요청
            /*AEPProjectileBase* Projectile = Cast<AEPProjectileBase>(PoolManager->GetObjectFromPool(
                ActorClassToSpawn));*/
        }
    }
}
