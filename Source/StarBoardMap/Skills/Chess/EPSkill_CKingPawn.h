// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/EPSkillBase.h"
#include "EPSkill_CKingPawn.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPSkill_CKingPawn : public UEPSkillBase
{
	GENERATED_BODY()
	
	virtual void Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex) override;
};
