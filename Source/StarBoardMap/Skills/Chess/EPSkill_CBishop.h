// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/EPSkillBase.h"
#include "EPSkill_CBishop.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPSkill_CBishop : public UEPSkillBase
{
	GENERATED_BODY()
	
	virtual void Activate(ACharacter* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex) override;

	void Explode(ACharacter* Caster, int32 Count);

	const FIntPoint TempXY[4] = { FIntPoint(1,1), FIntPoint(-1,1), FIntPoint(-1,-1), FIntPoint(1,-1) };
	bool IsValidXY[4];
	TSet<AActor*> ActorsToIgnore;
	TSet<AActor*> TargetActors;
};
