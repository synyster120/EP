// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EPHealthBarStatComponent.h"
#include "Data/EPCombatTypes.h"
#include "Data/EPCharacterTypes.h"

UEPHealthBarStatComponent::UEPHealthBarStatComponent()
{
}

FEPHealthInfo UEPHealthBarStatComponent::GetHealthInfo() const
{
    //Super::GetHealthInfo();

    FEPHealthInfo Info;
    Info.HealthType = EEPHealthType::HealthBar;
    Info.CurrentHealth = CurrentHealthBar;
    Info.MaxHealth = MaxHealthBar;
    Info.HealthRatio = (MaxHealthBar > 0.f) ? CurrentHealthBar / MaxHealthBar : 0.f;
    return Info;
}

void UEPHealthBarStatComponent::Initialize(const FEPBaseStat& BaseStatData)
{
    Super::Initialize(BaseStatData);

    MaxHealthBar = BaseStatData.MaxHealth;
    CurrentHealthBar = MaxHealthBar;
    UE_LOG(LogTemp, Warning, TEXT("MaxHealthBar: %f, CurrentHealthBar: %f, AttackRange: %f, AttackSpeed: %f, MovementSpeed: %f"), MaxHealthBar, CurrentHealthBar, AttackRange, AttackSpeed, MovementSpeed);

}

void UEPHealthBarStatComponent::ApplyDamage(const FEPDamageInfo& DamageInfo)
{
    Super::ApplyDamage(DamageInfo);
    if (IsDied()) return;

    // 기본 데미지를 가져옴
    float FinalDamage = DamageInfo.BaseDamage;

    //  FDamageInfo의 추가 정보를 활용하여 최종 데미지 계산
    // 예: 치명타였다면 데미지 2배
    if (DamageInfo.bIsCriticalHit)
    {
        FinalDamage *= 2.0f;
    }
    // 예: 캐릭터의 방어력 스탯(Defense)만큼 데미지 감소
    // FinalDamage -= DefenseStat;

    //const float OldHealth = CurrentHealth;
    // 현재 체력에서 최종 데미지를 차감
    CurrentHealthBar = FMath::Clamp(CurrentHealthBar - FinalDamage, 0.f, MaxHealthBar);

    // 자식들 체력 계산 및 적용

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
    UE_LOG(LogTemp, Warning, TEXT("Enemy == Type: %d, MaxHealthBar: %f, CurrentHealthBar: %f, AttackRange: %f, AttackSpeed: %f, MovementSpeed: %f"), ReactionType, MaxHealthBar, CurrentHealthBar, AttackRange, AttackSpeed, MovementSpeed);

    // 체력이 0 이하이면 죽음 델리게이트를 방송
    if (IsDied())
    {
        OnDied.Broadcast();
        return;
    }

    OnHitReact.Broadcast(ReactionType);
    OnHealthChanged.Broadcast();
}

bool UEPHealthBarStatComponent::IsDied() const
{
    //Super::IsDied();
    return CurrentHealthBar <= 0.0f;
}

void UEPHealthBarStatComponent::CalculateAndApplyDamage(const FEPDamageInfo& DamageInfo)
{
    //Super::CalculateAndApplyDamage(DamageInfo);
	// 이 컴포넌트는 오직 'float 체력 계산'이라는 자신의 책임에만 집중합니다.
	CurrentHealthBar = FMath::Clamp(CurrentHealthBar - DamageInfo.BaseDamage, 0.0f, MaxHealthBar);
}
