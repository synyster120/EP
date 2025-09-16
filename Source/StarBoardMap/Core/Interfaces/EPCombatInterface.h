// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/EPCharacterAnimationData.h"
#include "EPCombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UEPCombatInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *      전투(공격 피격 등) 수행 Interface
 */
class STARBOARDMAP_API IEPCombatInterface
{
    GENERATED_BODY()

public:
    // 데미지를 받는다는 계약
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    void TakeDamage(float DamageAmount, AController* InstigatorController, AActor* DamageCauser);

    // 죽음을 처리한다는 계약
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    void HandleDeath();

    // 피격 반응 애니메이션을 반환한다는 계약
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    UAnimMontage* GetHitReactionMontage(EEPHitReactionType HitReactionType);

};
