// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CKing.h"
#include "Components/EPStatComponent.h"
#include "Components/EPHealthBarStatComponent.h"
#include "Core/ChessGameMode.h"
#include "Skills/EPSkill_ExplodeSelf.h"
#include "Components/EPSkillComponent.h"

ACKing::ACKing()
{
}

void ACKing::BeginPlay()
{
    Name = FName("King");
    UnitType = 1;
    MovingTurn = true;
    Super::BeginPlay();
    UDataTable* MyTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/AssetDynamic/Data/Stats/DT_CharaterTypes_EnemyState.DT_CharaterTypes_EnemyState"));
    StatComponent->Initialize(*MyTable->FindRow<FEPBaseStat>(FName("CKing"), TEXT("EnemyStatContext")));
}

void ACKing::Attack()
{
    SkillComponent->ActivateSkill(0);
}

void ACKing::OnDied()
{
    Super::OnDied();

    if (StatComponent)
    {
        StatComponent->OnHealthChanged.Broadcast();
    }

    AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        UE_LOG(LogTemp, Warning, TEXT("gamemode clear"));
        GameMode->ClearGame();
    }
}

void ACKing::Warning()
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
		if(IsOnBoard(MiddlePoint)) MyController->SetGridWarning(MiddlePoint, 1); 
	}
}

FIntPoint ACKing::FindMove()
{
	FIntPoint TargetXY = NowXY;
	float BestScore = -FLT_MAX;
	AChessUnitController* MyController = Cast<AChessUnitController>(GetController());
	FVector PlayerVector = MyController->GetPlayerVector();

	for (const FIntPoint& Offset : AvaliablePoint)
	{
		FIntPoint NewXY = NowXY + Offset;

		if (MyController->GetGridState(NewXY) == 0)
		{
			FVector TargetLocation = MyController->GetGridVector(NewXY);

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

	FVector TargetVector = MyController->GetGridVector(TargetXY);
	float MinX = TargetVector.X - 75.f, MaxX = TargetVector.X + 75.f;
	float MinY = TargetVector.Y - 75.f, MaxY = TargetVector.Y + 75.f;
	bool IsInRange = (PlayerVector.X > MinX && PlayerVector.X < MaxX && PlayerVector.Y > MinY && PlayerVector.Y < MaxY) ? true : false;
	if (IsInRange) {
		TargetXY = NowXY;
	}

	return TargetXY;
}
