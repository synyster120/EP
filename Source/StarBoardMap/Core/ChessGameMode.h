// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Core/ChessGameState.h"
#include "AI/ChessUnitController.h"
#include "Chess/CKing.h"

#include "ChessGameMode.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API AChessGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	AChessGameMode();

	UPROPERTY()
	AChessGameState* ChessGameState;

	FTimerHandle TurnTimer;

	int32 PawnExplodeCounter;

	TArray<FIntPoint> RandomPoint;

protected:
	virtual void BeginPlay() override;

public:
	void StartGame();
	void ClearGame();

	void OnTurn();
	FIntPoint FindRandomMoveInPlace();
	

	int32 GetGridState(FIntPoint NewXY);
	FVector GetGridVector(FIntPoint NewXY);
	FVector GetPlayerVector();
};
