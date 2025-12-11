// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animation/AnimNotifies/EPNotifyState_AttackCollision.h"
#include "Characters/EPCombatCharacterBase.h"
#include "Components/EPSkillComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GenericTeamAgentInterface.h"

void UEPNotifyState_AttackCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    // [핵심] 오너 캐릭터 가져오기
    AActor* OwnerActor = MeshComp->GetOwner();
    AEPCombatCharacterBase* OwnerCharacter = Cast<AEPCombatCharacterBase>(OwnerActor);

    if (OwnerCharacter)
    {
        // 캐릭터의 스킬 컴포넌트(또는 SkillBase)에 "공격 추적 시작" 신호 전송
        // (충돌을 켤 소켓 이름, 예를 들어 "Weapon_Trace_Socket"을 넘겨줍니다)
        HitActors.Empty();

        AttackPhase = OwnerCharacter->SkillComponent->ReturnLastComboSkillIndex();
        SkillRangeData = OwnerCharacter->GetSkillComponent()->ReturnSkillRangeData();
        Dimensions = SkillRangeData.Dimensions;
        Damage = OwnerCharacter->GetSkillComponent()->ReturnDamage();
    }
}
void UEPNotifyState_AttackCollision::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (!MeshComp) return;
    DoAttackTrace(MeshComp);
}

void UEPNotifyState_AttackCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	UE_LOG(LogTemp, Log, TEXT("Attack %d End"), AttackPhase);
}

void UEPNotifyState_AttackCollision::DoAttackTrace(USkeletalMeshComponent* MeshComp)
{
    AActor* OwnerActor = MeshComp->GetOwner();
    AEPCombatCharacterBase* OwnerCharacter = Cast<AEPCombatCharacterBase>(OwnerActor);
    if (!OwnerCharacter) return;

    TArray<AActor*> AttachedActors;
    OwnerCharacter->GetAttachedActors(AttachedActors);
    FVector AttackOrigin = FVector::ZeroVector;

    if (MeshComp->DoesSocketExist(TEXT("SkillSocket")))
    {
        AttackOrigin = MeshComp->GetSocketLocation("SkillSocket");
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("'%s' --- 'SkillSocket'is null"), *MeshComp->GetName());
        return;
    }

    if (AttackOrigin != FVector::ZeroVector) {
        FQuat CapsuleRot = FRotationMatrix::MakeFromX(MeshComp->GetForwardVector()).ToQuat();

        DrawDebugSphere(
            MeshComp->GetWorld(),
            AttackOrigin,          // 캡슐 중심 = WeaponSocket 위치
            Dimensions.X,
            8,     // 회전 (Forward 방향)
            FColor::Red,  // 색상
            false,
            1.f             // 지속 시간
        );

        // 필요하다면 SweepMultiByChannel도 같은 파라미터로 돌릴 수 있음
        TArray<FHitResult> HitResults;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(MeshComp->GetOwner());

        bool bHit = MeshComp->GetWorld()->SweepMultiByChannel(
            HitResults,
            AttackOrigin,
            AttackOrigin, // 시작=끝 → 한 지점 캡슐 체크
            CapsuleRot,
            ECC_Pawn,
            FCollisionShape::MakeSphere(Dimensions.X),// MakeBox(Dimensions),
            Params
        );


        if (bHit)
        {
            for (auto& Hit : HitResults)
            {
                AEPCombatCharacterBase* HitCharacter = Cast<AEPCombatCharacterBase>(Hit.GetActor());

                // 유효성 검사(Null 체크, 자기 자신은 무시)
                if (!HitCharacter || HitCharacter == OwnerActor || !OwnerCharacter)
                {
                    continue;
                }

                // 팀 검사 - 적대적(Hostile) 여부 확인.  GetTeamAttitudeTowards(): 다르다면 무조건 적대 반환 (중립적 구별 못함)
                if (OwnerCharacter->GetTeamAttitudeTowards(*HitCharacter) == ETeamAttitude::Hostile && !HitActors.Contains(HitCharacter))
                {
                    HitActors.Add(HitCharacter);
                    UE_LOG(LogTemp, Warning, TEXT("[%s] AttackPhase : %d || Damage : %f || Hit: %s"), *OwnerCharacter->GetName(), AttackPhase, Damage, *HitCharacter->GetName());
                    UGameplayStatics::ApplyDamage(HitCharacter, Damage, OwnerCharacter->GetInstigatorController(), OwnerCharacter, UDamageType::StaticClass());
                }
            }
        }
    }
}