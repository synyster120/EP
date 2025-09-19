// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "Chess/CKing.h"
#include "Chess/CQueen.h"
#include "Chess/CBishop.h"
#include "Chess/CKnight.h"
#include "Chess/CRook.h"
#include "Chess/CPawn.h"

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

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACQueen> QueenBP;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACBishop> BishopBP;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACKnight> KnightBP;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACRook> RookBP;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACPawn> PawnBP;


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

	int32 Turn = 0;
	float TurnTime;

public: //function
	void BeginPlay();

	void SetGridState(FIntPoint OldXY, FIntPoint NewXY, int32 UnitType);
	int32 GetGridState(FIntPoint NewXY);

	int32 GetTurn() const { return Turn; }
	void OnTurn();

	TArray<ACUnit*> GetUnit() const { return Units; }

	FVector GetGridVector(FIntPoint NewXY);
};
