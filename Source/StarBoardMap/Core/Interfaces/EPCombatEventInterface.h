// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EPCombatEventInterface.generated.h"

/*
*		플루프린트에서 이벤트 및 구현 가능한 Combat Interface (상태를 바꾸는 행동)
*/

struct FEPDamageInfo;

UINTERFACE(MinimalAPI, Blueprintable)  // Blueprintable로 유지
class UEPCombatEventInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *      전투(공격 피격 등) 수행 Interface
 */
class STARBOARDMAP_API IEPCombatEventInterface
{
    GENERATED_BODY()

public:
    // 데미지를 받는다는 계약
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Event")
    void ApplyDamageInfo(const FEPDamageInfo& DamageInfo);
    
    // 죽음을 처리한다는 계약
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Event")
    void HandleDeath();

};
