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
	// 모든 자식 전략 클래스는 이 함수를 반드시 구현해야 함
	virtual bool FindTarget(ACharacter* Caster, const FEPSkillPhaseData& PhaseData, FEPSkillTargetData& OutTargetData) PURE_VIRTUAL(UEPTargetingStrategy::FindTarget, return false;);

};
