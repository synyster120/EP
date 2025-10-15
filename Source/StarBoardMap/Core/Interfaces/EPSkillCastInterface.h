// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EPSkillCastInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEPSkillCastInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STARBOARDMAP_API IEPSkillCastInterface
{
	GENERATED_BODY()

public:
	virtual bool RequiresMovementLock(int32 CurrentPhaseDataIndex) const { return false; }   // 기본 false
	virtual float GetWindupSeconds(int32 CurrentPhaseDataIndex) const { return 0.f; }        // 자폭/차지만 값 반환
};
