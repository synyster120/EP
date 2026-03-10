// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess/CUnit.h"
#include "CKing.generated.h"

class AChessUnitController;
class UEPStatComponent;
class UEPHealthBarStatComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKingDieDelegate, AActor*, DeadKing);

UCLASS()
class STARBOARDMAP_API ACKing : public ACUnit
{
	GENERATED_BODY()

	ACKing();

public:
	UPROPERTY(BlueprintAssignable)
	FOnKingDieDelegate OnKingDie;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	virtual void Attack() override;
	virtual void OnDied() override;
	virtual void Warning() override;

	virtual FIntPoint FindMove() override;
};
