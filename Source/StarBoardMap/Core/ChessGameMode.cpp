// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ChessGameMode.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

//#include "Data/EPWeaponTypes.h"

AChessGameMode::AChessGameMode()
{
}

void AChessGameMode::BeginPlay()
{
	ChessGameState = Cast<AChessGameState>(GetWorld()->GetGameState());

	for (int i = 1;i <= 9;i++)
	{
		for (int j = 1;j <= 9;j++)
		{
			RandomPoint.Add(FIntPoint(i, j));
		}
	}

	GetWorld()->GetTimerManager().SetTimer(TurnTimer, this, &AChessGameMode::StartGame, 1.0f, false);


	////weapon temp
	//UEPWeaponTypes* WeaponData = LoadObject<UEPWeaponTypes>(nullptr, TEXT("/Game/AssetDynamic/Data/Weapon/BP_WeaponTypes.BP_WeaponTypes"));
	//FWeaponInfo Data = WeaponData->GetWeaponInfoByName(FName("Hammer"));
	//GetWorld()->SpawnActor<AActor>(Data.WeaponBlueprint, FVector::ZeroVector, FRotator::ZeroRotator);

	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TempHandle,
		FTimerDelegate::CreateLambda([this]()
			{

				if (GameUIWidgetClass)
				{
					GameUIInstance = CreateWidget<UChessUserWidget>(GetWorld(), GameUIWidgetClass);
					if (GameUIInstance)
					{
						GameUIInstance->AddToViewport();
					}
				}
			}),
		0.5f,  // 딜레이 (초)
		false  // 반복 여부 (false = 한 번만 실행)
	);
}

void AChessGameMode::StartGame()
{
	GetWorld()->GetTimerManager().SetTimer(TurnTimer, this, &AChessGameMode::OnTurn, 3.0f, true);
}

void AChessGameMode::ClearGame()
{
	// 타이머 제거
	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void AChessGameMode::OnTurn()
{
	ChessGameState->OnTurn();

	for (ACUnit* Unit : ChessGameState->GetUnit()) 
	{
		AChessUnitController* Controller = Cast<AChessUnitController>(Unit->GetController());
		int32 NowTurn = ChessGameState->GetTurn();
		FIntPoint BeforeMovePoint = Unit->GetXY();
		FIntPoint MovingPlace = FIntPoint(-1,-1);

		if (NowTurn % 10 == 0) {
			if (Unit->GetUnitTurnType() == (NowTurn / 10 + 2) % 3 + 1) {
				MovingPlace = Controller->MoveOut();
			}
			else if (Unit->GetUnitName() == FName("Pawn")) {
				BeforeMovePoint = Controller->AttackPawn();
			}
		}
		else if (NowTurn % 10 == 6) {
			if (Unit->GetUnitName() == FName("King")) {
				Unit->SetMovingTurn(false);
			}
		}
		else if (NowTurn % 10 == 7) {
			if (Unit->GetUnitName() == FName("Pawn")) {
				MovingPlace = BeforeMovePoint = FindRandomMoveInPlace();
				Controller->SpawnPawn(MovingPlace);
			}
		}
		else if (NowTurn % 10 == 8){
			if (Unit->GetUnitName() == FName("King")) {
				Unit->SetMovingTurn(true);
			}
			else if (Unit->GetUnitName() == FName("Pawn")) {
				Unit->SetMovingTurn(true);
			}
		}
		else if (NowTurn % 10 == 9) {
			if (Unit->GetUnitName() == FName("Pawn")) {
				Unit->SetMovingTurn(false);
			}
		}

		if (Unit->GetMovingTurn())
		{
			MovingPlace = Controller->FindMove();
		}

		if (NowTurn % 10 == 1) {
			if (Unit->GetUnitTurnType() == NowTurn / 10 % 3 + 1) {
				MovingPlace = Controller->MoveIn();
			}
		}
		ChessGameState->SetGridState(BeforeMovePoint, MovingPlace, Unit->GetUnitType());
	}
}

FIntPoint AChessGameMode::FindRandomMoveInPlace()
{
	FIntPoint TargetXY;

	const int32 LastIndex = RandomPoint.Num() - 1;
	for (int32 i = 0; i <= LastIndex; i++)
	{
		int32 Index = FMath::RandRange(i, LastIndex);
		if (i != Index)
		{
			RandomPoint.Swap(i, Index);
		}
	}
	for (int32 i = 0;i <= LastIndex; i++)
	{
		if (ChessGameState->GetGridState(RandomPoint[i]) == 0) {
			TargetXY = RandomPoint[i];
		}
	}

	return TargetXY;
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

void AChessGameMode::SetGridWarning(FIntPoint NewXY, int32 Val)
{
	ChessGameState->SetGridWarning(NewXY, Val);
}
