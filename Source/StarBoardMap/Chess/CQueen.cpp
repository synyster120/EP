// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CQueen.h"
#include "AI/ChessUnitController.h"

ACQueen::ACQueen()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void ACQueen::BeginPlay()
{
    Name = FName("Queen");
    MovingTurn = true;
    UnitType = 2;
    Super::BeginPlay();
    UnitData = LoadObject<UChessUnitData>(nullptr, TEXT("/Game/AssetDynamic/Chess/BP_ChessUnitData.BP_ChessUnitData"));
    FUnitData Data = UnitData->FindUnitDataByName(FName(Name));

    QueenSpeed = Data.MovingSpeed;

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.1f;
}

void ACQueen::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AChessUnitController* MyController = Cast<AChessUnitController>(GetController());

    if (FVector::Dist(MyController->GetGridVector(NowXY+Direction), GetActorLocation()) < 100.f) {
        QueenMoveCounter += 1;
        if (QueenMoveMax == QueenMoveCounter) {
            QueenWarningGridSet();
            return;
        }
        MyController->SetGridWarning(NowXY, -UnitType);
        NowXY += Direction;
    }
}

void ACQueen::QueenWarningGridFunction(int32 Value, FIntPoint AddDirection)
{
    QueenMoveMax = Value;
    QueenMoveCounter = 0;
    Direction = AddDirection;

    //New
    NowXY += AddDirection;
    SetActorTickEnabled(true);

    /*GridSize = (FMath::Abs(Direction.X) + FMath::Abs(Direction.Y) == 1) ? 150.0f : 212.12f;

    float MoveInterval = GridSize / QueenSpeed;

    GetWorldTimerManager().ClearTimer(QueenWarningGridTimer);
    GetWorldTimerManager().SetTimer(
        QueenWarningGridTimer,
        this,
        &ACQueen::QueenWarningGridSet,
        MoveInterval,
        true,
        MoveInterval
    );*/
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

        if (MyController->GetGridState(NewXY) == 0 && MyController->IsOnBoard(NewXY)) {
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
    AChessUnitController* MyController = Cast<AChessUnitController>(GetController());
    NowXY += Direction;
    MyController->SetGridWarning(NowXY, -2);
    //QueenMoveCounter += 1;
    SetActorTickEnabled(false);

    if (QueenMoveCounter == QueenMoveMax) {
        //GetWorldTimerManager().ClearTimer(QueenWarningGridTimer);
        //SetXY(NowXY += Direction);
    }
}
