// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EPCharacterBase.h"
#include "Core/Interfaces/EPCombatInterface.h"
#include "EPCombatCharacterBase.generated.h"


// 전방 선언
class UEPStatComponent;
class UEPSkillComponent;
class UEPCharacterAnimationData;

/**
 *		IEPCombatInterface 상속받은 전투하는 CharacterBase
 */
UCLASS()
class STARBOARDMAP_API AEPCombatCharacterBase : public AEPCharacterBase, public IEPCombatInterface
{
	GENERATED_BODY()

public:
    AEPCombatCharacterBase();

    // ==== 컴포넌트 ====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UEPStatComponent> StatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UEPSkillComponent> SkillComponent;

    // ====== ICombatInterface Implementation ======
    // 인터페이스 함수의 기본 구현을 제공합니다.
    virtual void TakeDamage_Implementation(float DamageAmount, AController* InstigatorController, AActor* DamageCauser) override;
    virtual void HandleDeath_Implementation() override;
    virtual UAnimMontage* GetHitReactionMontage_Implementation(EEPHitReactionType HitReactionType) override;

protected:
    virtual void BeginPlay() override;

    /**
    * 캐릭터의 초기 데이터를 설정합니다.
    * BeginPlay에서 호출되며, 데이터 테이블로부터 스탯을 읽어 StatComponent를 초기화합니다.
    */
    virtual void InitializeCharacterData() override;

    // ... 데이터 테이블, 애니메이션 애셋 포인터 등 전투 관련 데이터는 모두 여기에 ...
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    TObjectPtr<UDataTable> StatDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    FName StatDataRowName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    TObjectPtr<UEPCharacterAnimationData> AnimDataAsset;
};
