// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

// FDefaultGameModuleImpl 대신 커스텀 클래스를 사용하기 위해 선언
class FStarBoardMapModule : public IModuleInterface
{
public:
    // 모듈이 메모리에 올라올 때 호출되는 함수
    virtual void StartupModule() override;

    // 모듈이 내려갈 때 호출되는 함수
    virtual void ShutdownModule() override;
};
