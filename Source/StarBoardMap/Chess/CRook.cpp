// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CRook.h"
#include "AI/ChessUnitController.h"
#include "Components/EPSkillComponent.h"

void ACRook::BeginPlay()
{
    Name = FName("Rook");
    UnitTurnType = 3;
    UnitType = 5;
    Super::BeginPlay();
}

void ACRook::Attack()
{
	SkillComponent->ActivateSkill(0);
}

void ACRook::Warning()
{
	AChessUnitController* MyController = Cast<AChessUnitController>(GetController());
	FIntPoint TempXY[4] = {
		FIntPoint(1,0),
		FIntPoint(-1, 0),
		FIntPoint(0, -1),
		FIntPoint(0, 1) };
	for (int32 i = 0;i < 4;i++) {
		FIntPoint MiddlePoint = NowXY;
		for (int32 j = 0;j < 8;j++) {
			MiddlePoint += TempXY[i];
			if (IsOnBoard(MiddlePoint)) {
				MyController->SetGridWarning(MiddlePoint, 4);
			}
			else break;
		}
	}
}
