// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Core/ChessGameState.h"
#include "AI/ChessUnitController.h"
#include "Chess/CKing.h"
#include "ChessUserWidget.h"

#include "EPChessGameplayManager.generated.h"

UCLASS()
class STARBOARDMAP_API AEPChessGameplayManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AEPChessGameplayManager();

private:
	/*UPROPERTY(VisibleAnywhere, Category = "GameState")
	AChessGameState* ChessGameState;*/

	FTimerHandle TurnTimer;

	TArray<FIntPoint> RandomPoint;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void StartGame();
	UFUNCTION()
	void ClearGame(AActor* DeadKing);

	void OnTurn();
	int32 GetTurn() const { return Turn; } // Game State
	FIntPoint FindRandomMoveInPlace();


	void SetGridState(FIntPoint OldXY, FIntPoint NewXY, int32 UnitType); // Game State
	int32 GetGridState(FIntPoint NewXY);

	FVector GetGridVector(FIntPoint NewXY);
	FVector GetPlayerVector();

	void SetGridWarning(FIntPoint NewXY, int32 Val);
	int32 GetGridWarning(FIntPoint NewXY) const { return GridWarningState[NewXY.X][NewXY.Y]; } // Game State


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UChessUserWidget> GameUIWidgetClass;

	UPROPERTY()
	UChessUserWidget* GameUIInstance;

	
// ---------------- Game State ----------------
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
	TSubclassOf<AActor> WarningTileBP;


	UPROPERTY(EditAnywhere)
	int32 BishopNum;
	UPROPERTY(EditAnywhere)
	int32 KnightNum;
	UPROPERTY(EditAnywhere)
	int32 RookNum;
	UPROPERTY(EditAnywhere)
	int32 PawnNum;

	UPROPERTY()
	TArray<ACUnit*> Units;

	UPROPERTY()
	TArray<AActor*> GridWarningTiles;

private: // State
	FIntPoint PlayerXY;
	int32 UnitState;
	int32 GridState[10][10];
	int32 GridWarningState[10][10];
	FVector GridVector[10][10];
	float GridSize = 150.f;

	bool bIsClear = false;

	int32 Turn = 0;
	float TurnTime;

protected:
	void GamestateSetup();

public:
	TArray<ACUnit*> GetUnit() const { return Units; }

	
// ---------------- Game Ending ----------------
public:
	// C++에서 호출하면 블루프린트에서 실행될 이벤트 (Door Open 연출 시작용)
	UFUNCTION(BlueprintImplementableEvent, Category = "GmaeEnding")
	void PlayOpenDoor();
};
