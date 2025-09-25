// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/EPSkillBase.h"
#include "EPSkill_MeleeInstant.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPSkill_MeleeInstant : public UEPSkillBase
{
	GENERATED_BODY()

public:
	virtual void Activate(ACharacter* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex) override;
};
