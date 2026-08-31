// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EPGameplayTags.h"
#include "GameplayTagsManager.h"

FEPGameplayTags FEPGameplayTags::GameplayTags;

void FEPGameplayTags::Initialize()
{
    // 게임 시작 시 한번만 호출하여, 문자열로 실제 태그를 찾아와 변수에 저장

    // 전투 관련 Tags
    GameplayTags.Tag_State_Dead = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("State.Dead"));
    GameplayTags.Tag_Action_Hit = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Action.Hit"));
    GameplayTags.Action_Hit_Default = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Action.Hit.Default")); 
    GameplayTags.Tag_Skill_Attack = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Skill.Attack"));
    
    // 상호작용 관련 Tags
    GameplayTags.Tag_InputUserSettings = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("InputUserSettings"));
    GameplayTags.Tag_InputUserSettings_PickUp = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("InputUserSettings.PickUp"));
    GameplayTags.Tag_InputUserSettings_Drop = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("InputUserSettings.Drop"));

    // UI 관련 Tags
    GameplayTags.Tag_UI_Menu_Pause = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("UI.Menu.Pause"));
    GameplayTags.Tag_UI_Menu_Settings = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("UI.Menu.Settings"));

}
