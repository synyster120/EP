// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/EPSkillBase.h"
#include "EPSkill_CRook.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPSkill_CRook : public UEPSkillBase
{
	GENERATED_BODY()

	virtual void Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex) override;

	void Explode(AActor* Caster, int32 Count);

	const FIntPoint TempXY[4] = { FIntPoint(1,0), FIntPoint(0,1), FIntPoint(-1,0), FIntPoint(0,-1) };
	bool IsValidXY[4];
	TSet<AActor*> ActorsToIgnore;
	TSet<AActor*> TargetActors;
};
