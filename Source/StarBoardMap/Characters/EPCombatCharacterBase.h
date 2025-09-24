// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EPCharacterBase.h"
#include "Core/Interfaces/EPCombatQueryInterface.h"
#include "Core/Interfaces/EPCombatEventInterface.h"
#include "Data/EPCombatTypes.h"
#include "EPCombatCharacterBase.generated.h"


// 전방 선언
class UEPStatComponent;
class UEPSkillComponent;
class UEPSkillDataAsset;
struct FEPDamageInfo;

/**
 *		IEPCombatInterface 상속받은 전투하는 CharacterBase
 */
UCLASS()
class STARBOARDMAP_API AEPCombatCharacterBase : public AEPCharacterBase, public IEPCombatEventInterface, public IEPCombatQueryInterface
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

    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintCallable, Category = "Mongtage")
    void HandleHitReaction(EEPHitReactionType HitReactionType);

    inline TObjectPtr<UEPSkillComponent> GetSkillComponent() { return SkillComponent; };

protected:
    virtual void BeginPlay() override;
    virtual void PostInitializeComponents() override;

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

};
