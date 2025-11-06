// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess/CUnit.h"
#include "CQueen.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API ACQueen : public ACUnit
{
	GENERATED_BODY()
		
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int32 QueenMoveCounter = 0, QueenMoveMax = 0;
	float QueenSpeed;
	FIntPoint Direction;

	FTimerHandle QueenWarningGridTimer;
	void QueenWarningGridSet();

public:
	void QueenWarningGridFunction(int32 Value, FIntPoint AddDirection);

	virtual FIntPoint FindMove();
};
