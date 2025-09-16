// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Data/Chess/ChessUnitData.h"
#include "CUnit.generated.h"

class AChessUnitController;

UCLASS()
class STARBOARDMAP_API ACUnit : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()	TSubclassOf<AChessUnitController> ControllerClass;
	UPROPERTY()	UChessUnitData* UnitData;

protected:
	FName Name;
	FIntPoint NowXY;
	TArray<FIntPoint> AvaliablePoint;
	FIntPoint OriginVector;
	bool MovingTurn = false;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetXY(FIntPoint NewXY);
	FIntPoint GetXY();
	FName GetUnitName();
	TArray<FIntPoint> GetAvaliablePoint();
	FUnitData GetUnitData();

	virtual void Attack();

	virtual void PossessedBy(AController* NewController) override;
};
