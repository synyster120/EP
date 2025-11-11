// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CPawn.h"
#include "AI/ChessUnitController.h"
#include "Components/EPSkillComponent.h"

void ACPawn::BeginPlay()
{
    Name = FName("Pawn");
    UnitType = 6;
    MovingTurn = false;
    Super::BeginPlay();
}

void ACPawn::Attack()
{
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}

void ACPawn::Warning()
{
	AChessUnitController* MyController = Cast<AChessUnitController>(GetController());
	FIntPoint TempXY[8] = {
		FIntPoint(0,1),
		FIntPoint(0, -1),
		FIntPoint(1, 1),
		FIntPoint(1, 0),
		FIntPoint(1,-1),
		FIntPoint(-1, -1),
		FIntPoint(-1,0),
		FIntPoint(-1,1) };

	for (int32 i = 0;i < 8;i++) {
		FIntPoint MiddlePoint = NowXY;
		MiddlePoint += TempXY[i];
		if (IsOnBoard(MiddlePoint)) MyController->SetGridWarning(MiddlePoint, 6);
	}
}
