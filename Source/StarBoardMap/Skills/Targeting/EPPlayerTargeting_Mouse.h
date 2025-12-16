// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/Targeting/EPTargetingStrategy.h"
#include "EPPlayerTargeting_Mouse.generated.h"

struct FEPSkillTargetData;
struct FEPSkillPhaseData;
/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPPlayerTargeting_Mouse : public UEPTargetingStrategy
{
	GENERATED_BODY()
	
public:
	virtual bool FindTarget(AActor* Caster, const FEPSkillPhaseData& PhaseData, FEPSkillTargetData& OutTargetData) override;
};
