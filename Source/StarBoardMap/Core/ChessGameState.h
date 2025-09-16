// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "Chess/CKing.h"

#include "ChessGameState.generated.h"


/**
 * 
 */
UCLASS()
class STARBOARDMAP_API AChessGameState : public AGameStateBase
{
	GENERATED_BODY()
	
private: // Unit
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACKing> KingBP;
	//Same below

	UPROPERTY(EditAnywhere)
	int32 BishopNum;
	UPROPERTY(EditAnywhere)
	int32 KnightNum;
	UPROPERTY(EditAnywhere)
	int32 RookNum;
	UPROPERTY(EditAnywhere)
	int32 PawnNum;

	TArray<ACUnit*> Units;

private: // State
	FIntPoint PlayerXY;
	int32 UnitState;
	int32 GridState[10][10];
	FVector GridVector[10][10];
	//FVector BasicGridVector;
	float GridSize = 150.f;

	bool bIsClear = false;

public: //State
	int32 Turn;
	float TurnTime;

public: //function
	void BeginPlay();

	FIntPoint GetPlayerXY();

	void SetGridState(FIntPoint OldXY, FIntPoint NewXY, int32 UnitType);
	int32 GetGridState(FIntPoint NewXY);

	FVector GetGridVector(FIntPoint NewXY);

	void SetUnitState();
	int32 GetUnitState();

	float GetGridSize();
	UPROPERTY()
	ACKing* SpawnedKing; // temp
};
