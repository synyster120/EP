// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/Targeting/EPTargetingStrategy.h"
#include "EPAITargeting_FindPlayer.generated.h"

struct FEPSkillTargetData;
struct FEPSkillPhaseData;
/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPAITargeting_FindPlayer : public UEPTargetingStrategy
{
	GENERATED_BODY()

public:
	virtual bool FindTarget(ACharacter* Caster, const FEPSkillPhaseData& PhaseData, FEPSkillTargetData& OutTargetData) override;

protected:
	// 블랙보드 키 
	static const FName TargetPlayerKey;
};
