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

    UWorld* World = GetWorld();
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    FRotator SpawnRotation = FRotator::ZeroRotator;

    FVector TempVector = FVector(ChessBoardActor->GetActorLocation().X - (GridSize * 4.5f), ChessBoardActor->GetActorLocation().Y - (GridSize * 4.5f), ChessBoardActor->GetActorLocation().Z + 1.f);
    for (int i = 0;i < 10;i++) {
        for (int j = 0;j < 10;j++) {
            GridVector[i][j] = FVector(TempVector.X + (GridSize * i), TempVector.Y + (GridSize * j), TempVector.Z);
            GridWarningTiles.Add(GetWorld()->SpawnActor<AActor>(WarningTileBP, GridVector[i][j], SpawnRotation, SpawnParams));
            GridWarningTiles[i * 10 + j]->SetActorHiddenInGame(true);
            GridWarningState[i][j] = 0;
            if (i == 0 || i == 9 || j == 0 || j == 9) GridState[i][j] = -1;
            else GridState[i][j] = 0;
        }
    }

    /////Spawn

    ACKing* TempKing = GetWorld()->SpawnActor<ACKing>(KingBP, GridVector[8][4], SpawnRotation, SpawnParams);
    TempKing->SetOriginPoint(FIntPoint(8, 4));
    GridState[8][4] = 1;
    Units.Add(TempKing);

    ACQueen* TempQueen = GetWorld()->SpawnActor<ACQueen>(QueenBP, GridVector[8][5], SpawnRotation, SpawnParams);
    TempQueen->SetOriginPoint(FIntPoint(8, 5));
    GridState[8][5] = 2;
    Units.Add(TempQueen);

    int32 TempX = 9, TempY = 4, MovingGrid = 1;

    for (int32 i = 0;i < BishopNum;i++) {
        ACBishop* TempBishop = GetWorld()->SpawnActor<ACBishop>(BishopBP, GridVector[TempX][TempY], SpawnRotation, SpawnParams);
        TempBishop->SetOriginPoint(FIntPoint(TempX, TempY));
        GridState[TempX][TempY] = 2;
        Units.Add(TempBishop);

        TempY += MovingGrid;
        MovingGrid *= (-1);
        MovingGrid > 0 ? MovingGrid += 1 : MovingGrid -= 1;
    }

    TempX = 4, TempY = 0, MovingGrid = 1;
    for (int32 i = 0;i < KnightNum;i++) {
        ACKnight* TempKnight = GetWorld()->SpawnActor<ACKnight>(KnightBP, GridVector[TempX][TempY], SpawnRotation, SpawnParams);
        TempKnight->SetOriginPoint(FIntPoint(TempX, TempY));
        GridState[TempX][TempY] = 2;
        Units.Add(TempKnight);

        TempX += MovingGrid;
        MovingGrid *= (-1);
        MovingGrid > 0 ? MovingGrid += 1 : MovingGrid -= 1;
    }
    
    TempX = 5, TempY = 9, MovingGrid = -1;
    for (int32 i = 0;i < RookNum;i++) {
        ACRook* TempRook = GetWorld()->SpawnActor<ACRook>(RookBP, GridVector[TempX][TempY], SpawnRotation, SpawnParams);
        TempRook->SetOriginPoint(FIntPoint(TempX, TempY));
        GridState[TempX][TempY] = 2;
        Units.Add(TempRook);

        TempX += MovingGrid;
        MovingGrid *= (-1);
        MovingGrid > 0 ? MovingGrid += 1 : MovingGrid -= 1;
    }
    
    TempX = 0, TempY = 0;
    for (int32 i = 0;i < PawnNum; i++) {
        ACPawn* TempPawn = GetWorld()->SpawnActor<ACPawn>(PawnBP, GridVector[TempX][TempY], SpawnRotation, SpawnParams);
        TempPawn->SetActorHiddenInGame(true);
        TempPawn->SetActorEnableCollision(false);
        Units.Add(TempPawn);
    }
}

void AChessGameState::SetGridState(FIntPoint OldXY, FIntPoint NewXY, int32 UnitType)
{
    GridState[OldXY.X][OldXY.Y] = 0;
    if (OldXY.X == 0 || OldXY.X == 9 || OldXY.Y == 0 || OldXY.Y == 9) GridState[OldXY.X][OldXY.Y] = -1;
    if (NewXY.X >= 0 && NewXY.X < 10 && NewXY.Y >= 0 && NewXY.Y < 10)GridState[NewXY.X][NewXY.Y] = UnitType;
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

void AChessGameState::OnTurn()
{
    Turn += 1;
}

FVector AChessGameState::GetGridVector(FIntPoint NewXY)
{
    return GridVector[NewXY.X][NewXY.Y];
}

void AChessGameState::SetGridWarning(FIntPoint NewXY, int32 Val)
{
    GridWarningState[NewXY.X][NewXY.Y] += Val;
    GridWarningTiles[NewXY.X*10 + NewXY.Y]->SetActorHiddenInGame(GridWarningState[NewXY.X][NewXY.Y] == 0 ? true : false);
}
