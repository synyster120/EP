// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ChessGameMode.h"
#include "Kismet/GameplayStatics.h"

AChessGameMode::AChessGameMode()
{
}

void AChessGameMode::BeginPlay()
{
	ChessGameState = Cast<AChessGameState>(GetWorld()->GetGameState());


    GetWorld()->GetTimerManager().SetTimer(TurnTimer, this, &AChessGameMode::StartGame, 1.0f, false);
}

void AChessGameMode::StartGame()
{
	GetWorld()->GetTimerManager().SetTimer(TurnTimer, this, &AChessGameMode::OnTurn, 3.0f, true);
}

void AChessGameMode::ClearGame()
{
}

void AChessGameMode::OnTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("OnTurn"));


	AChessUnitController* Controller = Cast<AChessUnitController>(ChessGameState->SpawnedKing->GetController());

	if (Cast<AChessUnitController>(ChessGameState->SpawnedKing->GetController()))
	{
		FIntPoint MovingPlace = Cast<AChessUnitController>(ChessGameState->SpawnedKing->GetController())->Move();
		ChessGameState->SetGridState(ChessGameState->SpawnedKing->GetXY(), MovingPlace, 1);
	}
}

FVector AChessGameMode::FindRandomSpawnPlace()
{
	return FVector();
}

FVector AChessGameMode::FindRandomMovePlace(int32 MoveState)
{
	return FVector();
}

int32 AChessGameMode::GetGridState(FIntPoint NewXY)
{
	return ChessGameState->GetGridState(NewXY);
}

FVector AChessGameMode::GetGridVector(FIntPoint NewXY)
{
	return ChessGameState->GetGridVector(NewXY);
}

FVector AChessGameMode::GetPlayerVector()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	APawn* PlayerPawn = PlayerController->GetPawn();
	return PlayerPawn->GetActorLocation();
}
