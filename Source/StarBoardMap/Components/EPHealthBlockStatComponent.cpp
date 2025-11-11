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

    // ���� ���� �ʱ�ȭ
    //InitializeCommonStats(BaseStatData);
}

void UEPHealthBlockStatComponent::ApplyDamage(const FEPDamageInfo& DamageInfo)
{
    Super::ApplyDamage(DamageInfo);
    if (IsDied()) return;

    if (DamageInfo.BaseDamage <= 0) return;

    // �⺻ �������� ������
    float FinalDamage = 1;

    // ġ��Ÿ���ٸ� ������ 2��
    if (DamageInfo.bIsCriticalHit)
    {
        FinalDamage *= 2;
    }

    // ĳ������ ���� ����(Defense)��ŭ ������ ����
    // FinalDamage -= DefenseStat;

    // ���� ü�¿��� ���� �������� ����
    CurrentHealthBlocks = (int)FMath::Clamp(CurrentHealthBlocks - FinalDamage, 0.f, MaxHealthBlocks);

    // ���� �������� ���� �ǰ� ���� Ÿ���� ����
    EEPHitReactionType ReactionType = EEPHitReactionType::Light;
    if (FinalDamage > 50.0f) // �� ������ ���� StatComponent�� ���
    {
        ReactionType = EEPHitReactionType::Heavy;
    }
    else
    {
        ReactionType = EEPHitReactionType::Light;
    }

    // �ǰ� ����Ʈ/����� ���? (�ִ� ��Ƽ����?)
    UE_LOG(LogTemp, Warning, TEXT("Player == Type: %d MaxHealthBlocks: %d, CurrentHealthBlocks: %d, AttackRange: %f, AttackSpeed: %f, MovementSpeed: %f"), ReactionType, MaxHealthBlocks, CurrentHealthBlocks, AttackRange, AttackSpeed, MovementSpeed);

    // ü���� 0 �����̸� ���� ��������Ʈ�� ���
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
    OnHealthChanged.Broadcast();
}

bool UEPHealthBlockStatComponent::IsDied() const
{
    //Super::IsDied();
    return CurrentHealthBlocks <= 0;
}

void UEPHealthBlockStatComponent::CalculateAndApplyDamage(const FEPDamageInfo& DamageInfo)
{
    //Super::CalculateAndApplyDamage(DamageInfo);
    // �� ������Ʈ�� ���� 'ĭ ���� ü�� ���'�̶�� �ڽ��� å�ӿ��� �����մϴ�.
    const int32 DamageInBlocks = FMath::CeilToInt(DamageInfo.BaseDamage / 50.0f);
    CurrentHealthBlocks = FMath::Clamp(CurrentHealthBlocks - DamageInBlocks, 0, MaxHealthBlocks);

}
