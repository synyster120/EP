// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EPGameplayTags.h"
#include "GameplayTagsManager.h"

FEPGameplayTags FEPGameplayTags::GameplayTags;

void FEPGameplayTags::Initialize()
{
    // 게임 시작 시 한번만 호출하여, 문자열로 실제 태그를 찾아와 변수에 저장
    GameplayTags.AI_State_Patrol = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("AI.State.Patrol"));
    GameplayTags.AI_State_Combat_Chase = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("AI.State.Combat.Chase"));
    GameplayTags.AI_State_Combat_Attack = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("AI.State.Combat.Attack"));
}
