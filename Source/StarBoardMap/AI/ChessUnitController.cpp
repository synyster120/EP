// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ChessUnitController.h"
#include "Data/Chess/ChessUnitData.h"

#include "Chess/Core/EPChessGameplayManager.h"
#include "EngineUtils.h"

AChessUnitController::AChessUnitController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BB = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
}

void AChessUnitController::SetDefaultInformation()
{
	FUnitData Data = Unit->GetUnitData();
	BB->SetValueAsName(FName("Name"), Data.Name);
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

	for (TActorIterator<AEPChessGameplayManager> It(GetWorld()); It; ++It)
	{
		ChessGameManager = *It;
		break;
	}

    //ChessGameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	
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
	TargetPoint = Unit->FindMove();


	Move(TargetPoint, false);
	
	return TargetPoint;
}

void AChessUnitController::Move(FIntPoint TargetPoint, bool IsBigJump)
{
	if (!IsValid(ChessGameManager)) return;

	if(Unit->GetUnitName() != FName("Pawn") && Unit->GetUnitName() != FName("Queen")) ChessGameManager->SetGridWarning(TargetPoint, Unit->GetUnitType());
	if(Unit->GetUnitName() != FName("Queen")) Unit->SetXY(TargetPoint);
	BB->SetValueAsBool(FName("IsMoving"), false);
	BB->SetValueAsBool(FName("IsBigJump"), IsBigJump);
	BB->SetValueAsVector(FName("TargetPoint"), ChessGameManager->GetGridVector(TargetPoint));
	BB->SetValueAsInt(FName("NowState"), 1);
}

FIntPoint AChessUnitController::MoveIn()
{
	if (!IsValid(ChessGameManager)) return FIntPoint::ZeroValue;

	Unit->SetMovingTurn(true);
	FIntPoint TargetPoint = ChessGameManager->FindRandomMoveInPlace();
	
	Move(TargetPoint, true);

	return TargetPoint;
}

FIntPoint AChessUnitController::MoveOut()
{
	if (!IsValid(ChessGameManager)) return FIntPoint::ZeroValue;

	Unit->SetMovingTurn(false);
	FIntPoint TargetPoint = Unit->GetOriginPoint();

	Move(TargetPoint, true);

	return TargetPoint;
}

void AChessUnitController::MoveEnd()
{
	if (!IsValid(ChessGameManager)) return;

	ChessGameManager->SetGridWarning(Unit->GetXY(), -Unit->GetUnitType());
}

void AChessUnitController::SpawnPawn(FIntPoint SpawnPoint)
{
	if (!IsValid(ChessGameManager)) return;

	Unit->SetXY(SpawnPoint);
	ChessGameManager->SetGridWarning(SpawnPoint, Unit->GetUnitType());
	FVector SpawnVector = ChessGameManager->GetGridVector(SpawnPoint);
	SpawnVector.Z += 500.f;
	Unit->SetActorLocation(SpawnVector);
	Unit->SetActorHiddenInGame(false);
	Unit->SetActorEnableCollision(true);
}

FIntPoint AChessUnitController::Attack()
{
	Unit->Attack();
	return Unit->GetXY();
}

int32 AChessUnitController::GetGridState(FIntPoint XY)
{
	if (!IsValid(ChessGameManager)) return 0;

	return ChessGameManager->GetGridState(XY);
}

FVector AChessUnitController::GetPlayerVector()
{
	if (!IsValid(ChessGameManager)) return FVector::ZeroVector;

	return ChessGameManager->GetPlayerVector();
}

FVector AChessUnitController::GetGridVector(FIntPoint NewXY)
{
	if (!IsValid(ChessGameManager)) return FVector::ZeroVector;

	return ChessGameManager->GetGridVector(NewXY);
}

void AChessUnitController::SetGridWarning(FIntPoint XY, int32 Value)
{
	if (!IsValid(ChessGameManager)) return;

	ChessGameManager->SetGridWarning(XY, Value);
}

void AChessUnitController::DestroyUnit()
{

}