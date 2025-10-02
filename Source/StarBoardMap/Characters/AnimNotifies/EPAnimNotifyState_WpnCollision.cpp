// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AnimNotifies/EPAnimNotifyState_WpnCollision.h"
#include "Kismet/GameplayStatics.h"  
#include "GameFramework/Character.h"
#include "Components/EPSkillComponent.h"
#include "Characters/EPPlayerCharacter.h"
#include "Skills/EPSkillBase.h"

void UEPAnimNotifyState_WpnCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{

    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(MeshComp->GetWorld(), 0);
    if (MyCharacter) {
        AEPPlayerCharacter* Player = Cast<AEPPlayerCharacter>(MyCharacter);
        AttackPhase = Player->SkillComponent->ReturnLastComboSkillIndex(); //플레이어 스택 갖고오기

        UE_LOG(LogTemp, Log, TEXT("Attack %d Begin"), AttackPhase);
        HitEnemies.Empty();

        SkillRangeData = Player->SkillComponent->ReturnSkillRangeData();
        Dimensions = SkillRangeData.Dimensions;
        Damage = Player->SkillComponent->ReturnDamage();
        //현재 공격 데미지 가져오기
    }
}

void UEPAnimNotifyState_WpnCollision::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    if(MeshComp) DoAttackTrace(MeshComp);
}

void UEPAnimNotifyState_WpnCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UE_LOG(LogTemp, Log, TEXT("Attack %d End"), AttackPhase);
}

void UEPAnimNotifyState_WpnCollision::DoAttackTrace(USkeletalMeshComponent* MeshComp)
{
    UWorld* World = MeshComp->GetWorld();

    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(MeshComp->GetWorld(), 0);
    if (!MyCharacter) return;

    TArray<AActor*> AttachedActors;
    MyCharacter->GetAttachedActors(AttachedActors);
    FVector AttackOrigin = FVector::ZeroVector;
    for (AActor* Child : AttachedActors)
    {
        if (Child->ActorHasTag("Weapon")) // 태그로 구분
        {
            UStaticMeshComponent* TargetMeshComp = Child->FindComponentByClass<UStaticMeshComponent>();
            AttackOrigin = TargetMeshComp->GetSocketLocation("AttackSocket");
        }
    }

    if (AttackOrigin != FVector::ZeroVector) {
        FQuat CapsuleRot = FRotationMatrix::MakeFromX(MeshComp->GetForwardVector()).ToQuat();

        // Sweep 대신 그냥 DrawDebugCapsule로 "무기 범위용 캡슐"만 시각화
        DrawDebugBox(
            World,
            AttackOrigin,          // 캡슐 중심 = WeaponSocket 위치
            Dimensions,
            CapsuleRot,     // 회전 (Forward 방향)
            FColor::Green,  // 색상
            false,
            1.f             // 지속 시간
        );

        // 필요하다면 SweepMultiByChannel도 같은 파라미터로 돌릴 수 있음
        TArray<FHitResult> HitResults;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(MeshComp->GetOwner());

        bool bHit = World->SweepMultiByChannel(
            HitResults,
            AttackOrigin,
            AttackOrigin, // 시작=끝 → 한 지점 캡슐 체크
            CapsuleRot,
            ECC_Pawn,
            FCollisionShape::MakeBox(Dimensions),
            Params
        );

        if (bHit)
        {
            for (auto& Hit : HitResults)
            {
                AActor* Enemy = Hit.GetActor();
                if (Enemy && !HitEnemies.Contains(Enemy))
                {
                    HitEnemies.Add(Enemy);
                    UE_LOG(LogTemp, Warning, TEXT("Attack %d Damage %f Hit: %s"), AttackPhase, Damage, *Enemy->GetName());
                    //UGameplayStatics::ApplyDamage(Enemy, PhaseData.Damage, GetOwner()->GetInstigatorController(), this, nullptr);
                }
            }
        }
    }
}
