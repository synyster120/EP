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

FIntPoint AChessUnitController::FindMove()
{
	TArray<FIntPoint> AvaliablePoint = Unit->GetAvaliablePoint();
	FIntPoint NowPoint = Unit->GetXY();
	FIntPoint TargetPoint = NowPoint;

	//Unit Move
	if (Unit->GetUnitName() == FName("King")) TargetPoint = FindKingMove(AvaliablePoint);
	else TargetPoint = FindUnitMove(AvaliablePoint);


	Move(TargetPoint, false);
	
	return TargetPoint;
}

void AChessUnitController::Move(FIntPoint TargetPoint, bool IsBigJump)
{
	Unit->SetXY(TargetPoint);
	BB->SetValueAsBool(FName("IsMoving"), false);
	BB->SetValueAsBool(FName("IsBigJump"), IsBigJump);
	BB->SetValueAsVector(FName("TargetPoint"), ChessGameMode->GetGridVector(TargetPoint));
	BB->SetValueAsInt(FName("NowState"), 1);
}

FIntPoint AChessUnitController::MoveIn()
{
	Unit->SetMovingTurn(true);
	FIntPoint TargetPoint = ChessGameMode->FindRandomMoveInPlace();
	
	Move(TargetPoint, true);

	return TargetPoint;
}

FIntPoint AChessUnitController::MoveOut()
{
	Unit->SetMovingTurn(false);
	FIntPoint TargetPoint = Unit->GetOriginPoint();

	Move(TargetPoint, true);

	return TargetPoint;
}

FIntPoint AChessUnitController::FindKingMove(TArray<FIntPoint> AvaliablePoint)
{
	FIntPoint TargetXY = Unit->GetXY();
	float BestScore = -FLT_MAX;
	FVector PlayerVector = ChessGameMode->GetPlayerVector();

	for (const FIntPoint& Offset : AvaliablePoint)
	{
		FIntPoint NewXY = Unit->GetXY() + Offset;

        if (ChessGameMode->GetGridState(NewXY) == 0)
		{
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

FIntPoint AChessUnitController::FindQueenMove(FIntPoint Direction)
{
	FIntPoint TargetXY = Unit->GetXY();

	for (int32 i = 0;i < FMath::RandRange(1, 9); i++)
	{
		FIntPoint NewXY = TargetXY + Direction;

		if (ChessGameMode->GetGridState(NewXY) == 0) TargetXY = NewXY;
		else break;
	}

	return TargetXY;
}

FIntPoint AChessUnitController::FindUnitMove(TArray<FIntPoint> AvaliablePoint)
{
	FIntPoint TargetXY = Unit->GetXY();

	const int32 LastIndex = AvaliablePoint.Num() - 1;
	for (int32 j = 0;j < 3;j++) {
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				AvaliablePoint.Swap(i, Index);
			}
		}
	}

	for (const FIntPoint& Offset : AvaliablePoint)
	{
		FIntPoint NewXY = Unit->GetXY() + Offset;

		if (ChessGameMode->GetGridState(NewXY) == 0)
		{
			TargetXY = NewXY;
			break;
		}
	}

	if (Unit->GetUnitName() == FName("Queen")) TargetXY = FindQueenMove(TargetXY - Unit->GetXY());

	return TargetXY;
}

void AChessUnitController::SpawnPawn(FIntPoint SpawnPoint)
{
	FVector SpawnVector = ChessGameMode->GetGridVector(SpawnPoint);
	SpawnVector.Z += 500.f;
	Unit->SetXY(SpawnPoint);
	Unit->SetActorLocation(SpawnVector);
	Unit->SetActorHiddenInGame(false);
	Unit->SetActorEnableCollision(true);
}

FIntPoint AChessUnitController::AttackPawn()
{
	UE_LOG(LogTemp, Warning, TEXT("HIHIATACKPAWN"));
	Unit->SetActorHiddenInGame(true);
	Unit->SetActorEnableCollision(false);
	return Unit->GetXY();
}

void AChessUnitController::DestroyUnit()
{

}