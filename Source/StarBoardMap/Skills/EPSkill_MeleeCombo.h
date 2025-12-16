// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/EPSkillBase.h"
#include "EPSkill_MeleeCombo.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPSkill_MeleeCombo : public UEPSkillBase
{
	GENERATED_BODY()
	
public: 
	virtual void Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex) override;
};
