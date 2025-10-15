// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EPHealthBlockStatComponent.h"
#include "Data/EPCombatTypes.h"
#include "Data/EPCharacterTypes.h"

UEPHealthBlockStatComponent::UEPHealthBlockStatComponent()
{
}

FEPHealthInfo UEPHealthBlockStatComponent::GetHealthInfo() const
{
    //Super::GetHealthInfo();
    FEPHealthInfo Info;
    Info.HealthType = EEPHealthType::HealthBlock;
    Info.CurrentBlocks = CurrentHealthBlocks;
    Info.MaxBlocks = MaxHealthBlocks;
    Info.HealthRatio = (MaxHealthBlocks > 0) ? (float)CurrentHealthBlocks / (float)MaxHealthBlocks : 0.f;
    return Info;
}

void UEPHealthBlockStatComponent::Initialize(const FEPBaseStat& BaseStatData)
{
    Super::Initialize(BaseStatData);
    MaxHealthBlocks = BaseStatData.MaxHealth;
    CurrentHealthBlocks = MaxHealthBlocks;

    //UE_LOG(LogTemp, Warning, TEXT("MaxHealthBlocks: %d, CurrentHealthBlocks: %d, AttackRange: %d, AttackSpeed: %d, MovementSpeed: %d"), MaxHealthBlocks, CurrentHealthBlocks, AttackRange, AttackSpeed, MovementSpeed);
    UE_LOG(LogTemp, Warning, TEXT("MaxHealthBlocks: %d, CurrentHealthBlocks: %d, AttackRange: %f, AttackSpeed: %f, MovementSpeed: %f"), MaxHealthBlocks, CurrentHealthBlocks, AttackRange, AttackSpeed, MovementSpeed);

    // 공통 스탯 초기화
    //InitializeCommonStats(BaseStatData);
}

void UEPHealthBlockStatComponent::ApplyDamage(const FEPDamageInfo& DamageInfo)
{
    Super::ApplyDamage(DamageInfo);
    if (IsDied()) return;

    if (DamageInfo.BaseDamage <= 0) return;

    // 기본 데미지를 가져옴
    float FinalDamage = 1;

    // 치명타였다면 데미지 2배
    if (DamageInfo.bIsCriticalHit)
    {
        FinalDamage *= 2;
    }

    // 캐릭터의 방어력 스탯(Defense)만큼 데미지 감소
    // FinalDamage -= DefenseStat;

    // 현재 체력에서 최종 데미지를 차감
    CurrentHealthBlocks = (int)FMath::Clamp(CurrentHealthBlocks - FinalDamage, 0.f, MaxHealthBlocks);

    // 최종 데미지에 따라 피격 반응 타입을 결정
    EEPHitReactionType ReactionType = EEPHitReactionType::Light;
    if (FinalDamage > 50.0f) // 이 로직은 이제 StatComponent가 담당
    {
        ReactionType = EEPHitReactionType::Heavy;
    }
    else
    {
        ReactionType = EEPHitReactionType::Light;
    }

    // 피격 이펙트/사운드는 어디서? (애님 노티파이?)
    UE_LOG(LogTemp, Warning, TEXT("Player == Type: %d MaxHealthBlocks: %d, CurrentHealthBlocks: %d, AttackRange: %f, AttackSpeed: %f, MovementSpeed: %f"), ReactionType, MaxHealthBlocks, CurrentHealthBlocks, AttackRange, AttackSpeed, MovementSpeed);

    // 체력이 0 이하이면 죽음 델리게이트를 방송
    if (IsDied())
    {
        OnDied.Broadcast();
        return;
    }

    OnHitReact.Broadcast(ReactionType);
    if (MaxHealthBlocks != CurrentHealthBlocks)
    {
        OnHealthChanged_Two.Broadcast((float)CurrentHealthBlocks, (float)MaxHealthBlocks);
    }
}

bool UEPHealthBlockStatComponent::IsDied() const
{
    //Super::IsDied();
    return CurrentHealthBlocks <= 0;
}

void UEPHealthBlockStatComponent::CalculateAndApplyDamage(const FEPDamageInfo& DamageInfo)
{
    //Super::CalculateAndApplyDamage(DamageInfo);
    // 이 컴포넌트는 오직 '칸 단위 체력 계산'이라는 자신의 책임에만 집중합니다.
    const int32 DamageInBlocks = FMath::CeilToInt(DamageInfo.BaseDamage / 50.0f);
    CurrentHealthBlocks = FMath::Clamp(CurrentHealthBlocks - DamageInBlocks, 0, MaxHealthBlocks);

}
