// Fill out your copyright notice in the Description page of Project Settings.

#include "StarBoardMap.h"
#include "Modules/ModuleManager.h"
#include "Core/EPGameplayTags.h"

void FStarBoardMapModule::StartupModule()
{
    // 모듈이 시작될 때 태그 초기화
    FEPGameplayTags::Initialize();
    UE_LOG(LogTemp, Log, TEXT("StarBoardMap Module Started: Native Tags Initialized"));
}

void FStarBoardMapModule::ShutdownModule()
{
    // [필요하다면] 정리 로직
}

IMPLEMENT_PRIMARY_GAME_MODULE( FStarBoardMapModule, StarBoardMap, "StarBoardMap" ); // FDefaultGameModuleImpl -> FStarBoardMapModule
