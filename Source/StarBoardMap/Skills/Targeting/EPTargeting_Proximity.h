// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/Targeting/EPTargetingStrategy.h"
#include "EPTargeting_Proximity.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPTargeting_Proximity : public UEPTargetingStrategy
{
	GENERATED_BODY()

public:
	virtual bool FindTarget(AActor* Caster, const FEPSkillPhaseData& PhaseData, FEPSkillTargetData& OutTargetData) override;
};
