// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EPCombatCharacterBase.h"
#include "EPEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API AEPEnemyCharacter : public AEPCombatCharacterBase
{
	GENERATED_BODY()
	
public:
	AEPEnemyCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void InitializeCharacterData() override;

protected:
	float PerceptionRadius;
	float LosePerceptionRadius;
};
