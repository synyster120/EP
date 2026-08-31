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

    // -- 전투 관련 Tags --
    FGameplayTag Tag_State_Dead;
    FGameplayTag Tag_Action_Hit;
    FGameplayTag Action_Hit_Default; 
    FGameplayTag Tag_Skill_Attack; 

    // -- 상호작용 관련 Tags --
    FGameplayTag Tag_InputUserSettings;
    FGameplayTag Tag_InputUserSettings_PickUp;
    FGameplayTag Tag_InputUserSettings_Drop;

    // -- UI 관련 Tags --
    FGameplayTag Tag_UI_Menu_Pause;
    FGameplayTag Tag_UI_Menu_Settings;

private:
    static FEPGameplayTags GameplayTags;
};