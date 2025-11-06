// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Data/Chess/ChessUnitData.h"
#include "Characters/EPEnemyCharacter.h"
#include "CUnit.generated.h"

class AChessUnitController;
class UEPSkillComponent;

UCLASS()
class STARBOARDMAP_API ACUnit : public AEPEnemyCharacter
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
	int32 UnitTurnType = 0;
	int32 UnitType = 0;
	FIntPoint NowXY;
	FIntPoint OriginPoint;
	TArray<FIntPoint> AvaliablePoint;
	bool MovingTurn = false;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetXY(FIntPoint NewXY);
	FIntPoint GetXY();
	FName GetUnitName();
	int32 GetUnitType() const { return UnitType; }
	int32 GetUnitTurnType() const { return UnitTurnType; }
	TArray<FIntPoint> GetAvaliablePoint();
	FUnitData GetUnitData();
	bool GetMovingTurn() const { return MovingTurn; }
	void SetMovingTurn(bool TurnState);
	void SetOriginPoint(FIntPoint NewOriginPoint);
	FIntPoint GetOriginPoint() const { return OriginPoint; }
	bool IsOnBoard(FIntPoint XY);

	virtual void Attack();
	virtual FIntPoint FindMove();
	virtual void Warning();

	virtual void PossessedBy(AController* NewController) override;
};
