#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EPSkillComponent.generated.h"

// 전방 선언
class UEPStatComponent;
class UEPSkillBase;
class UEPSkillDataAsset;
struct FEPSkillTargetData;
//class UEPSkillTypes;
struct FEPSkillRangeData;
class UEPTargetingStrategy;

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillCooldownEnded, int32, SkillIndex, UObject*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementLockEnded);

USTRUCT(BlueprintType)
struct FSkillRuntimeData
{
    GENERATED_BODY()

    // 캐릭터가 소유하는 스킬 객체들의 배열
    UPROPERTY()
    TObjectPtr<UEPSkillBase> SkillObject = nullptr;

    // 활성화된 스킬쿨타임 타이머들을 관리 (스킬 ID, 타이머 핸들)
    UPROPERTY()
    FTimerHandle CooldownTimerHandle;

    // 이 스킬의 최대 콤보 수
    int32 MaxComboCount;
    // 나중에 필요한 데이터들을 여기에 추가...
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARBOARDMAP_API UEPSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    // 생성자
    UEPSkillComponent();

    // 스킬 시스템을 초기화 함수 (UI를 초기화하거나, 버프 시스템과 연동하는 등 확장성 위해 CreateSkills()와 분리)
    void InitializeSkills(const TArray<TSoftObjectPtr<UEPSkillDataAsset>>& SkillAssets);

    /**
     * 지정된 인덱스의 스킬을 발동시킵니다.
     * @param SkillIndex 발동할 스킬의 인덱스 (Skills 배열 기준)
     * @param TargetData 스킬 발동에 필요한 타겟 정보
     */
    UFUNCTION(BlueprintCallable)
    void ActivateSkill(int32 SkillIndex);
    void ActivateSkillFinished(int32 SkillIndex);

    bool CanActivateSkill(int32 SkillIndex);

    int32 ReturnLastComboSkillIndex() const { return LastComboSkillIndex; }
    
    FEPSkillRangeData ReturnSkillRangeData();
    float ReturnDamage();

    // 기존 킬 객체들이나 실행 중이던 쿨타임 타이머 정리 함수
    void ClearSkills();

protected:
    virtual void BeginPlay() override;

private:
    // 스킬 데이터 에셋을 기반으로 실제 스킬 인스턴스들 생성 함수
    void CreateSkills(const TArray<TSoftObjectPtr<UEPSkillDataAsset>>& SkillAssets);

    // 스킬 사용 후 쿨타임 시작 함수 (스킬 사용 불가능)
    void StartCooldown(FName SkillID);

    // 스킬 쿨타임이 종료되었을 때 호출 함수 (스킬 사용 가능)
    void OnCooldownFinished(FName SkillID);

    // 콤보 유효 시간 타이머 세팅
    void StartComboWindow(int32 SkillIndex, int32 CurrentComboIndex);

    // 콤보 유효 시간 타이머 리셋 함수
    UFUNCTION()
    void ResetCombo();

    bool PerformTargeting(UEPSkillBase* SkillToActivate, int32 SkillIndex, FEPSkillTargetData& OutTargetData);

public:
    FOnSkillCooldownEnded OnSkillCooldownEnded;
    FOnMovementLockEnded OnMovementLockEnded;

    // 타겟팅 방식 지정
    UPROPERTY(EditAnywhere, Instanced, Category = "Combat")
    TObjectPtr<UEPTargetingStrategy> TargetingStrategy;

protected:
    // 타겟팅 방식 객체
    /*UPROPERTY()
    UEPTargetingStrategy* CachedStrategy;*/

private:
    // 소유 스킬 객체 / 스킬 쿨 타이머 배열 (데이터의 원본)
    UPROPERTY(VisibleAnywhere, Category = "Skill")
    TArray<FSkillRuntimeData> SkillSlots;

    // SkillID를 배열 인덱스로 변환해주는 조회용 맵 (빠른 접근용)
    UPROPERTY()
    TMap<FName, int32> SkillIDToIndexMap;

    // Key: 콤보 상태를 추적할 스킬슬롯의 Index (Index)
    // Value: 현재 진행 중인 콤보 단계 (Index)
    UPROPERTY()
    TMap<int32, int32> ComboStateMap;

    // 콤보 유효시간 타이머. 마지막으로 사용된 콤보 스킬슬롯의 Index 저장
    FTimerHandle ComboTimerHandle;
    int32 LastkillSlotIndex = 0;
    int32 LastComboSkillIndex = 0;

    FTimerHandle WindupHandle;

    // 소유자(캐릭터)의 StatComponent에 대한 참조 (BeginPlay 시점에 찾아옴)
    /*UPROPERTY()
    TObjectPtr<UEPStatComponent> StatComponentRef;*/
};
