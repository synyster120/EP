// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ChessUnitController.h"
#include "Core/ChessGameMode.h"
#include "Data/Chess/ChessUnitData.h"

AChessUnitController::AChessUnitController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BB = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
}

void AChessUnitController::SetDefaultInformation()
{
	FUnitData Data = Unit->GetUnitData();
	BB->SetValueAsFloat(FName("JumpHeight"), Data.JumpHeight);
	BB->SetValueAsInt(FName("AttackProbability"), Data.AttackProbability);
	BB->SetValueAsFloat(FName("StopTime1"), Data.StopTime1);
	BB->SetValueAsFloat(FName("StopTime2"), Data.StopTime2);
	BB->SetValueAsFloat(FName("MovingSpeed"), Data.MovingSpeed);
	BB->SetValueAsFloat(FName("FallingSpeed"), Data.FallingSpeed);
	BB->SetValueAsFloat(FName("BigMovingSpeed"), Data.BigMovingSpeed);
	BB->SetValueAsFloat(FName("BigFallingSpeed"), Data.BigFallingSpeed);
	BB->SetValueAsInt(FName("NowState"), 0);
	BB->SetValueAsBool(FName("IsMoving"), false);
	BB->SetValueAsVector(FName("OriginVector"), Unit->GetActorLocation());
}

void AChessUnitController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ACUnit* PossessedUnit = Cast<ACUnit>(InPawn))
	{
		Unit = PossessedUnit;
	}

	if (BlackboardAsset && BehaviorAsset)
	{
		if (UseBlackboard(BlackboardAsset, BB))
		{
			RunBehaviorTree(BehaviorAsset);
		}
	}
	
    ChessGameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	
	FTimerHandle TimerHandle_Init;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_Init,
		this,
		&AChessUnitController::SetDefaultInformation,
		0.1f,
		false
	);
}

FIntPoint AChessUnitController::Move()
{
	TArray<FIntPoint> AvaliablePoint = Unit->GetAvaliablePoint();
	FIntPoint NowPoint = Unit->GetXY();
	FIntPoint TargetPoint = NowPoint;

	//Unit Move
	if (Unit->GetUnitName() == FName("King")) TargetPoint = FindKingMove(AvaliablePoint);
	else if (Unit->GetUnitName() == FName("Queen")) TargetPoint = FindQueenMove(AvaliablePoint);


	Unit->SetXY(TargetPoint);
	BB->SetValueAsBool(FName("IsMoving"), false);
	BB->SetValueAsBool(FName("IsBigJump"), false);
	BB->SetValueAsVector(FName("TargetPoint"), ChessGameMode->GetGridVector(TargetPoint));
	BB->SetValueAsInt(FName("NowState"), 1);
	
	return TargetPoint;
}

FIntPoint AChessUnitController::MoveIn()
{
	return FIntPoint();
}

FIntPoint AChessUnitController::MoveOut()
{
	return FIntPoint();
}

FIntPoint AChessUnitController::FindKingMove(TArray<FIntPoint> AvaliablePoint)
{
	FIntPoint TargetXY = Unit->GetXY();
	float BestScore = -FLT_MAX;
	FVector PlayerVector = ChessGameMode->GetPlayerVector();

	for (const FIntPoint& Offset : AvaliablePoint)
	{
		FIntPoint NewXY = Unit->GetXY() + Offset;

        if (ChessGameMode->GetGridState(NewXY) != 0)
        {
            continue;
        }
        else {
            FVector TargetLocation = ChessGameMode->GetGridVector(NewXY);

			float Score = 0.0f;

			float PlayerDist = FVector::Dist2D(TargetLocation, PlayerVector);
			Score -= PlayerDist;

			if (Score > BestScore)
			{
				BestScore = Score;
				TargetXY = NewXY;
			}
        }
	}

	FVector TargetVector = ChessGameMode->GetGridVector(TargetXY);
	float MinX = TargetVector.X - 75.f, MaxX = TargetVector.X + 75.f;
	float MinY = TargetVector.Y - 75.f, MaxY = TargetVector.Y + 75.f;
	bool IsInRange = (PlayerVector.X > MinX && PlayerVector.X < MaxX && PlayerVector.Y > MinY && PlayerVector.Y < MaxY) ? true : false;
	if (IsInRange) {
		TargetXY = Unit->GetXY();
	}

	return TargetXY;
}

FIntPoint AChessUnitController::FindQueenMove(TArray<FIntPoint> AvaliablePoint)
{
	return FIntPoint();
}

FIntPoint AChessUnitController::FindUnitMove(TArray<FIntPoint> AvaliablePoint)
{
	return FIntPoint();
}

void AChessUnitController::DestroyUnit()
{

}