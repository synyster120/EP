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
	void MoveEnd();

	void SpawnPawn(FIntPoint SpawnPoint);
	FIntPoint Attack();
	FIntPoint GetXY() const { return Unit->GetXY(); }
	int32 GetUnitType() const { return Unit->GetUnitType(); }
	int32 GetGridState(FIntPoint XY);
	FVector GetPlayerVector();
	FVector GetGridVector(FIntPoint NewXY);
	void SetGridWarning(FIntPoint XY, int32 Value);

	void Warning() const { Unit->Warning(); }
	bool IsOnBoard(FIntPoint XY) const { return Unit->IsOnBoard(XY); }

	void DestroyUnit();
};
