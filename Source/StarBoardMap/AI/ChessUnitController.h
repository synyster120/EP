// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Chess/CUnit.h"
#include "Data/Chess/ChessUnitData.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ChessUnitController.generated.h"

class AChessGameMode;

UCLASS()
class STARBOARDMAP_API AChessUnitController : public AAIController
{
	GENERATED_BODY()
	
private:

	UPROPERTY()	ACUnit* Unit;
	UPROPERTY() AChessGameMode* ChessGameMode;
	UPROPERTY(EditAnywhere)	UBlackboardData* BlackboardAsset;
	UPROPERTY(EditAnywhere)	UBehaviorTree* BehaviorAsset;
	UPROPERTY()	UBehaviorTreeComponent* BehaviorComp;
	UPROPERTY()	UBlackboardComponent* BB;


protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	AChessUnitController();

	void SetDefaultInformation();

	FIntPoint FindMove();
	void Move(FIntPoint TargetPoint, bool IsBigJump);
	FIntPoint MoveIn();
	FIntPoint MoveOut();

	FIntPoint FindKingMove(TArray<FIntPoint> AvaliablePoint);
	FIntPoint FindQueenMove(FIntPoint Direction);
	FIntPoint FindUnitMove(TArray<FIntPoint> AvaliablePoint);
	void SpawnPawn(FIntPoint SpawnPoint);
	FIntPoint AttackPawn();

	void DestroyUnit();
};
