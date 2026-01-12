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

    // -- Character 관련 태그들 --
    FGameplayTag Tag_State_Dead;
    FGameplayTag Action_Hit_Default; 
    FGameplayTag Tag_InputUserSettings;
    FGameplayTag Tag_Skill_Attack;

private:
    static FEPGameplayTags GameplayTags;
};