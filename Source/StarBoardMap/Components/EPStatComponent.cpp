
#include "Components/EPStatComponent.h"
#include "Data/EPCharacterTypes.h"

UEPStatComponent::UEPStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEPStatComponent::Initialize(const FEPBaseStat& BaseStatData)
{
    // '설계도'의 값을 내부 변수로 복사
    MaxHealth = BaseStatData.MaxHealth;
    AttackRange = BaseStatData.AttackRange;
    AttackSpeed = BaseStatData.AttackSpeed;
    MovementSpeed = BaseStatData.MovementSpeed;

    // 런타임 변수 초기화
    CurrentHealth = MaxHealth;

    // 부가 스탯 맵 초기화
    SecondaryStats.Empty();

    // 초기 상태를 UI 등에 알리기 위해 델리게이트 한번 호출
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

// 데미지를 적용하고 체력을 변경
void UEPStatComponent::ApplyDamage(float DamageAmount)
{
    if (DamageAmount <= 0.0f || CurrentHealth <= 0.0f)
    {
        return;
    }

    const float PrevHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

    UE_LOG(LogTemp, Log, TEXT("Damage Applied: %f, Health: %f -> %f"), DamageAmount, PrevHealth, CurrentHealth);

    // 체력 변경 '방송' -> UI, 캐릭터 애니메이션 등 구독자들이 이 신호를 받아 각자 행동함
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

    // 체력이 0 이하면 사망 '방송'
    if (CurrentHealth <= 0.0f)
    {
        OnDied.Broadcast();
    }
}

// 버프/디버프 갱신 함수
void UEPStatComponent::ModifySecondaryStat(ESecondaryStatType StatType, float Amount)
{
    // 1. FindOrAdd: 맵에서 StatType에 해당하는 값 찾음
    //    만약 값이 없다면, 기본값(float의 경우 0.0f)으로 새로 추가하고 그 값을 가져옴
    float& StatValue = SecondaryStats.FindOrAdd(StatType);

    const float OldValue = StatValue;
    StatValue += Amount;

    UE_LOG(LogTemp, Log, TEXT("Secondary Stat '%s' changed: %f -> %f (Delta: %f)"),
        *UEnum::GetDisplayValueAsText(StatType).ToString(), // Enum 이름을 깔끔하게 출력
        OldValue,
        StatValue,
        Amount);

    // 만약 스탯 값이 0에 가까워졌다면 맵에서 아예 제거하여 깔끔하게 유지 (0.0001 같은 값 제거)
    if (FMath::IsNearlyZero(StatValue))
    {
        SecondaryStats.Remove(StatType);
        UE_LOG(LogTemp, Log, TEXT("Secondary Stat '%s' removed from map as it became zero."), *UEnum::GetDisplayValueAsText(StatType).ToString());
    }
}

// 부가 스탯 값 받아옴 (없다면 0.0f)
float UEPStatComponent::GetSecondaryStat(ESecondaryStatType StatType) const
{
    // 맵에 해당 스탯이 존재하면 값을, 없으면 0.0f를 반환
    const float* FoundStat = SecondaryStats.Find(StatType);
    if (FoundStat)
    {
        return *FoundStat;
    }
    return 0.0f;
}

void UEPStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

