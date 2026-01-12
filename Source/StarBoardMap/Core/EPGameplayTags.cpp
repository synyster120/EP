// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EPGameplayTags.h"
#include "GameplayTagsManager.h"

FEPGameplayTags FEPGameplayTags::GameplayTags;

void FEPGameplayTags::Initialize()
{
    // 게임 시작 시 한번만 호출하여, 문자열로 실제 태그를 찾아와 변수에 저장
    GameplayTags.Tag_State_Dead = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("State.Dead"));
    GameplayTags.Action_Hit_Default = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Action.Hit.Default")); 
    GameplayTags.Tag_InputUserSettings = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("InputUserSettings"));
    GameplayTags.Tag_Skill_Attack = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Skill.Attack"));
}
