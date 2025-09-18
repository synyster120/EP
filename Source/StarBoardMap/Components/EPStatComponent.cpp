
#include "Components/EPStatComponent.h"
#include "Data/EPCharacterTypes.h"
#include "Data/EPCombatTypes.h"


// 버프/디버프 갱신 함수
void UEPStatComponent::ModifySecondaryStat(EEPSecondaryStatType StatType, float Amount)
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
float UEPStatComponent::GetSecondaryStat(EEPSecondaryStatType StatType) const
{
    // 맵에 해당 스탯이 존재하면 값을, 없으면 0.0f를 반환
    const float* FoundStat = SecondaryStats.Find(StatType);
    if (FoundStat)
    {
        return *FoundStat;
    }
    return 0.0f;
}

void UEPStatComponent::CommonApplyDamage(const FEPDamageInfo& DamageInfo)
{
    //if (OldHealth != CurrentHealth) // 체력이 실제로 변경되었을 때
    //{
    //    // 체력 변경 '방송' -> UI, 캐릭터 애니메이션 등 구독자들이 이 신호를 받아 각자 행동함
    //    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
    //}


    // 결정된 피격 반응 타입을 델리게이트를 통해 외부에 방송(Broadcast)
    //    "이런 반응을 해야 해!" 라고 신호만 보낼 뿐, 애니메이션 재생은 신경쓰지 않습니다.
    //OnHitReact.Broadcast(ReactionType);

    
}

//void UEPStatComponent::InitializeCommonStats(const FEPBaseStat& BaseStatData)
//{
//    // '설계도'의 값을 내부 변수로 복사
//    AttackRange = BaseStatData.AttackRange;
//    AttackSpeed = BaseStatData.AttackSpeed;
//    MovementSpeed = BaseStatData.MovementSpeed;
//
//    // 부가 스탯 맵 초기화
//    SecondaryStats.Empty();
//
//    // 초기 상태를 UI 등에 알리기 위해 델리게이트 한번 호출
//    OnHealthChanged.Broadcast();
//}


void UEPStatComponent::Initialize(const FEPBaseStat& BaseStatData)
{
    // '설계도'의 값을 내부 변수로 복사
    AttackRange = BaseStatData.AttackRange;
    AttackSpeed = BaseStatData.AttackSpeed;
    MovementSpeed = BaseStatData.MovementSpeed;

    // 부가 스탯 맵 초기화
    SecondaryStats.Empty();

    // 초기 상태를 UI 등에 알리기 위해 델리게이트 한번 호출
    OnHealthChanged.Broadcast();
}

void UEPStatComponent::ApplyDamage(const FEPDamageInfo& DamageInfo)
{
    float temp = DamageInfo.BaseDamage;
    UE_LOG(LogTemp, Warning, TEXT("ApplyDamage ---> Damage : %f" ), temp);
    // 자식 클래스에서 재정의될 것이므로, 기본 구현은 비워둡니다.
}

bool UEPStatComponent::IsDied() const
{
    // 자식이 재정의하지 않았을 경우를 대비한 안전한 기본값 반환
    return true;
}

FEPHealthInfo UEPStatComponent::GetHealthInfo() const
{
    // 안전한 기본값 반환
    return FEPHealthInfo();
}

void UEPStatComponent::CalculateAndApplyDamage(const FEPDamageInfo& DamageInfo)
{
    // 자식 클래스에서 재정의될 것이므로, 기본 구현은 비워둡니다.
}