// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EPCharacterBase.h"
#include "Core/Interfaces/EPCombatQueryInterface.h"
#include "Core/Interfaces/EPCombatEventInterface.h"
#include "Data/EPCombatTypes.h"
#include "GenericTeamAgentInterface.h" // team interface
#include "EPCombatCharacterBase.generated.h"


// 전방 선언
class UEPStatComponent;
class UEPSkillComponent;
class UEPSkillDataAsset;
struct FEPDamageInfo;
class UEPTargetingStrategy;

/**
 *		IEPCombatInterface 상속받은 전투하는 CharacterBase
 */

UCLASS(Abstract)
class STARBOARDMAP_API AEPCombatCharacterBase : public AEPCharacterBase, public IEPCombatEventInterface, public IEPCombatQueryInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
    AEPCombatCharacterBase();

    // ==== 컴포넌트 ====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UEPStatComponent> StatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UEPSkillComponent> SkillComponent;

    // ====== ICombat Event Interface Implementation ======
    virtual void ApplyDamageInfo_Implementation(const FEPDamageInfo& DamageInfo) override;
    UFUNCTION()
    virtual void HandleDeath_Implementation() override;
    // ====== ICombat Query Interface Implementation ======
    virtual UAnimMontage* GetHitReactionMontage(EEPHitReactionType HitReactionType) override;

    // interface
    virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }


    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
    
    // 피격 데미지 바인딩 (추상화)함수
    UFUNCTION()
    virtual void HandleHealthChanged(float NewHealth, float MaxHealth) PURE_VIRTUAL(AEPCombatCharacterBase::HandleHealthChanged, );

    // Die 바인딩 (추상화)함수
    UFUNCTION()
    virtual void OnDied() PURE_VIRTUAL(AEPCombatCharacterBase::OnDied, );

    // 몽타주 플레이 (추상화)함수
    UFUNCTION()
    virtual void CurrentMontagePlay(UAnimMontage* CurrentMontage, EEPCombatMontageType CurrentMontageType) PURE_VIRTUAL(AEPCombatCharacterBase::CurrentMontagePlay, );

    UFUNCTION(BlueprintCallable, Category = "Mongtage")
    void HandleHitReaction(EEPHitReactionType HitReactionType);

    FORCEINLINE TObjectPtr<UEPSkillComponent> GetSkillComponent() { return SkillComponent; };
    FORCEINLINE UEPStatComponent* GetStatComponent() const { return StatComponent; }

    // 이 캐릭터가 사용할 타겟팅 전략 클래스 (Character 블루프린트에서 지정)
    UPROPERTY(EditDefaultsOnly, Category = "Data")
    TSubclassOf<UEPTargetingStrategy> TargetingStrategyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
    FGenericTeamId TeamID;

    FORCEINLINE void SetCurrentInteractionMontage(TObjectPtr<UAnimMontage> CurrentMontage) { CurrentInteractionMontage = CurrentMontage; };

protected:
    virtual void BeginPlay() override;
    virtual void PostInitializeComponents() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /**
    * 캐릭터의 초기 데이터를 설정합니다.
    * BeginPlay에서 호출되며, 데이터 테이블로부터 스탯을 읽어 StatComponent를 초기화합니다.
    */
    virtual void InitializeCharacterData() override;

    // ... 데이터 테이블, 애니메이션 애셋 포인터 등 전투 관련 데이터는 모두 여기에 ...
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    FDataTableRowHandle StatDataRowHandle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    TArray<TSoftObjectPtr<UEPSkillDataAsset>> DefaultSkills;

    // 현재 재생 중인 인터랙션 몽타주를 추적하기 위한 포인터
    UPROPERTY()
    TObjectPtr<UAnimMontage> CurrentInteractionMontage;
};
