// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess/CUnit.h"
#include "CBishop.generated.h"

class AChessUnitController;

UCLASS()
class STARBOARDMAP_API ACBishop : public ACUnit
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Attack() override;
	virtual void Warning() override;
};
