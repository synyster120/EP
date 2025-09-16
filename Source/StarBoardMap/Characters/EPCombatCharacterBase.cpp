// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EPCombatCharacterBase.h"
#include "Components/EPStatComponent.h"
#include "Components/EPSkillComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/EPCharacterTypes.h"

AEPCombatCharacterBase::AEPCombatCharacterBase()
{
    // 메쉬(SkeletalMeshComponent) 설정
    // 캡슐의 Z축 아래로 90도 내리고, 캡슐을 기준으로 90도 회전
    GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -90.0));
    GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

    StatComponent = CreateDefaultSubobject<UEPStatComponent>(TEXT("StatComponent"));
    SkillComponent = CreateDefaultSubobject<UEPSkillComponent>(TEXT("SkillComponent"));
}

void AEPCombatCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    // BeginPlay는 게임이 시작될 때 호출됩니다.
    // 여기서 캐릭터의 데이터를 초기화하는 함수를 호출하는 것이 일반적입니다.
    InitializeCharacterData();
}

void AEPCombatCharacterBase::InitializeCharacterData()
{
    Super::InitializeCharacterData();

    // 스탯 초기화
    if (StatDataTable) // 캐릭터가 자신의 스탯 데이터 테이블을 가지고 있다고 가정
    {
        // 데이터 테이블에서 Stat 데이터를 찾아옵니다.
        FEPBaseStat* StatData = StatDataTable->FindRow<FEPBaseStat>(StatDataRowName, TEXT(""));
        if (StatData && StatComponent)
        {
            // StatData로 StatComponent 초기화
            StatComponent->Initialize(*StatData);

            // tatComponent의 OnDied 델리게이트에 HandleDeath 함수를 바인딩
            StatComponent->OnDied.AddDynamic(this, &AEPCombatCharacterBase::HandleDeath_Implementation);
        }
    }

}

// 피격 처리 함수(계산)
void AEPCombatCharacterBase::TakeDamage_Implementation(float DamageAmount, AController* InstigatorController, AActor* DamageCauser)
{
    if (!StatComponent) return;

    // 데미지 량에 따라 피격 반응 타입 결정 (예시 로직)
    EEPHitReactionType ReactionType = EEPHitReactionType::Light;
    if (DamageAmount > 50.0f)
    {
        ReactionType = EEPHitReactionType::Heavy;
    }

    // 인터페이스 함수를 호출하여 데이터 애셋에서 알맞은 몽타주를 가져옴
    UAnimMontage* ReactionMontage = GetHitReactionMontage_Implementation(ReactionType);
    if (ReactionMontage)
    {
        PlayAnimMontage(ReactionMontage);
    }

    // StatComponent에 최종 데미지 적용
    StatComponent->ApplyDamage(DamageAmount);
}

// 죽음 처리 함수
void AEPCombatCharacterBase::HandleDeath_Implementation()
{
    // 데이터 애셋에서 죽음 애니메이션 몽타주를 가져와 재생
    if (AnimDataAsset && AnimDataAsset->DeathAnimationMontage)
    {
        PlayAnimMontage(AnimDataAsset->DeathAnimationMontage.LoadSynchronous());
    }

    // 추가적인 죽음 처리 로직 (콜리전 끄기 등)
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// 피격 타입 맞는 몽타주 검색 및 반환 함수
UAnimMontage* AEPCombatCharacterBase::GetHitReactionMontage_Implementation(EEPHitReactionType HitReactionType)
{
    // 데이터 애셋의 TMap에서 HitReactionType에 맞는 몽타주를 찾아 반환
    if (AnimDataAsset && AnimDataAsset->HitReactionMontages.Contains(HitReactionType))
    {
        return AnimDataAsset->HitReactionMontages[HitReactionType].LoadSynchronous();
    }
    return nullptr;
}
