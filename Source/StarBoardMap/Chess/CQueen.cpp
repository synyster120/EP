// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CQueen.h"
#include "AI/ChessUnitController.h"

void ACQueen::BeginPlay()
{
    Name = FName("Queen");
    MovingTurn = true;
    UnitType = 2;
    Super::BeginPlay();
    UnitData = LoadObject<UChessUnitData>(nullptr, TEXT("/Game/AssetDynamic/Chess/BP_ChessUnitData.BP_ChessUnitData"));
    FUnitData Data = UnitData->FindUnitDataByName(FName(Name));

    QueenSpeed = Data.MovingSpeed;
}

void ACQueen::QueenWarningGridFunction(int32 Value, FIntPoint AddDirection)
{
    QueenMoveMax = Value - 1;
    QueenMoveCounter = 0;
    float GridSize;
    Direction = AddDirection;

    GridSize = (FMath::Abs(Direction.X) + FMath::Abs(Direction.Y) == 1) ? 150.0f : 212.12f;

    float MoveInterval = GridSize / QueenSpeed;

    GetWorldTimerManager().ClearTimer(QueenWarningGridTimer);
    GetWorldTimerManager().SetTimer(
        QueenWarningGridTimer,
        this,
        &ACQueen::QueenWarningGridSet,
        MoveInterval,
        true,
        MoveInterval
    );
}

FIntPoint ACQueen::FindMove()
{
    FIntPoint TargetXY = Super::FindMove();
    Direction = TargetXY - NowXY;
    AChessUnitController* MyController = Cast<AChessUnitController>(GetController());

    TargetXY = NowXY;
    int32 MaxRange = FMath::RandRange(1, 9);
    QueenMoveCounter = 0;

    for (int32 i = 0;i < MaxRange; i++)
    {
        FIntPoint NewXY = TargetXY + Direction;

        if (MyController->GetGridState(NewXY) == 0) {
            TargetXY = NewXY;
            MyController->SetGridWarning(NewXY, 2);
            QueenMoveCounter += 1;
        }
        else break;
    }
    if (QueenMoveCounter > 1) {
        QueenWarningGridFunction(QueenMoveCounter, Direction);
    }
    else {
        SetXY(TargetXY);
    }

    return TargetXY;
}

void ACQueen::QueenWarningGridSet()
{
    AChessUnitController* QueenController = Cast<AChessUnitController>(GetController());
    NowXY += Direction;
    QueenController->QueenArrivePoint(NowXY);
    QueenMoveCounter += 1;

    if (QueenMoveCounter == QueenMoveMax) {
        GetWorldTimerManager().ClearTimer(QueenWarningGridTimer);
        SetXY(NowXY += Direction);
    }
}
