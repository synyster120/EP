// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * 
 */

struct FEPGameplayTags
{
public:
    static const FEPGameplayTags& Get() { return GameplayTags; }
    static void Initialize();

    // -- AI 상태 태그들 --
    FGameplayTag AI_State_Patrol;
    FGameplayTag AI_State_Combat_Chase;
    FGameplayTag AI_State_Combat_Attack;

private:
    static FEPGameplayTags GameplayTags;
};