// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CBishop.h"
#include "AI/ChessUnitController.h"
#include "Components/EPSkillComponent.h"

void ACBishop::BeginPlay()
{
    Name = FName("Bishop");
    UnitTurnType = 1;
    UnitType = 3;
    Super::BeginPlay();
}

void ACBishop::Attack()
{
	SkillComponent->ActivateSkill(0);
}

void ACBishop::Warning()
{
	AChessUnitController* MyController = Cast<AChessUnitController>(GetController());
	FIntPoint TempXY[4] = {
		FIntPoint(1,1),
		FIntPoint(-1, -1),
		FIntPoint(1, -1),
		FIntPoint(-1, 1) };
	for (int32 i = 0;i < 4;i++) {
		FIntPoint MiddlePoint = NowXY;
		for (int32 j = 0;j < 8;j++) {
			MiddlePoint += TempXY[i];
			if (IsOnBoard(MiddlePoint)) {
				MyController->SetGridWarning(MiddlePoint, 3);
			}
			else break;
		}
	}
}
