// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CUnit.h"
#include "UObject/ConstructorHelpers.h"
#include "AI/ChessUnitController.h"

// Sets default values
ACUnit::ACUnit()
{
	static ConstructorHelpers::FClassFinder<AChessUnitController> ControllerBPClass(TEXT("/Game/AssetDynamic/Chess/BP_ChessUnitController"));
	if (ControllerBPClass.Succeeded())
	{
		AIControllerClass = ControllerBPClass.Class;
		AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	}
}

// Called when the game starts or when spawned
void ACUnit::BeginPlay()
{
	Super::BeginPlay();
}
void ACUnit::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

FUnitData ACUnit::GetUnitData()
{
	UnitData = LoadObject<UChessUnitData>(nullptr, TEXT("/Game/AssetDynamic/Chess/BP_ChessUnitData.BP_ChessUnitData"));
	FUnitData Data = UnitData->FindUnitDataByName(FName(Name));

	for (int i = 0;i < Data.AvaliablePoint.Num();i++) {
		AvaliablePoint.Add(Data.AvaliablePoint[i]);
	}

	return Data;
}

// Called to bind functionality to input
void ACUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACUnit::SetXY(FIntPoint NewXY)
{
	NowXY = NewXY;
}

FIntPoint ACUnit::GetXY()
{
	return NowXY;
}

FName ACUnit::GetUnitName()
{
	return Name;
}

TArray<FIntPoint> ACUnit::GetAvaliablePoint()
{
	return AvaliablePoint;
}

void ACUnit::Attack()
{
}