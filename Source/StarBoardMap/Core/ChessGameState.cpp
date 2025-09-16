// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ChessGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "AI/ChessUnitController.h"

void AChessGameState::BeginPlay()
{
    /////Chess Grid Vector init
    FString TargetActorName = TEXT("ChessBoard_GEN_VARIABLE_BP_ChessBoardBox_C_CAT_0");
    AActor* ChessBoardActor = nullptr;

    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

    // 이름 비교로 액터 찾기
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == TargetActorName)
        {
            ChessBoardActor = Actor;
            break;
        }
    }
    
    FVector TempVector = FVector(ChessBoardActor->GetActorLocation().X - (GridSize * 4.5f), ChessBoardActor->GetActorLocation().Y - (GridSize * 4.5f), ChessBoardActor->GetActorLocation().Z);
    for (int i = 0;i < 10;i++) {
        for (int j = 0;j < 10;j++) {
            GridVector[i][j] = FVector(TempVector.X + (GridSize * i), TempVector.Y + (GridSize * j), TempVector.Z);
            GridState[i][j] = 0;
        }
    }

    /////Spawn
    if (KingBP)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            FRotator SpawnRotation = FRotator::ZeroRotator;

            SpawnedKing = GetWorld()->SpawnActor<ACKing>(KingBP, GridVector[8][4], SpawnRotation, SpawnParams);
            SpawnedKing->SetXY(FIntPoint(8, 4));
            SpawnedKing->SpawnDefaultController();
        }
    }
}

FIntPoint AChessGameState::GetPlayerXY()
{
	return PlayerXY;
}

void AChessGameState::SetGridState(FIntPoint OldXY, FIntPoint NewXY, int32 UnitType)
{
    GridState[OldXY.X][OldXY.Y] = 0;
    GridState[NewXY.X][NewXY.Y] = UnitType;
}

int32 AChessGameState::GetGridState(FIntPoint NewXY)
{
    int32 XX = NewXY.X;
    int32 YY = NewXY.Y;
    if (XX > 0 && XX < 9 && YY > 0 && YY < 9) {
        return GridState[XX][YY];
    }
    return -1;
}

FVector AChessGameState::GetGridVector(FIntPoint NewXY)
{
    return GridVector[NewXY.X][NewXY.Y];
}

void AChessGameState::SetUnitState()
{
}

int32 AChessGameState::GetUnitState()
{
	return int32();
}

float AChessGameState::GetGridSize()
{
	return 0.0f;
}
