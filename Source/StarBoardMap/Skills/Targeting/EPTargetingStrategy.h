// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EPTargetingStrategy.generated.h"

struct FEPSkillTargetData;
struct FEPSkillPhaseData;
/**
 * 
 */
UCLASS(Abstract)
class STARBOARDMAP_API UEPTargetingStrategy : public UObject
{
	GENERATED_BODY()

public:
	// 추상 함수
	virtual bool FindTarget(ACharacter* Caster, const FEPSkillPhaseData& PhaseData, FEPSkillTargetData& OutTargetData) PURE_VIRTUAL(UEPTargetingStrategy::FindTarget, return false;);

};
