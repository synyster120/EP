// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/Core/EPChessGameplayManager.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

#include "Data/EPFXPreloadLibrary.h"

// Sets default values
AEPChessGameplayManager::AEPChessGameplayManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEPChessGameplayManager::BeginPlay()
{
	Super::BeginPlay();

	GamestateSetup();

	for (int i = 1; i <= 9; i++)
	{
		for (int j = 1; j <= 9; j++)
		{
			RandomPoint.Add(FIntPoint(i, j));
		}
	}

	auto* FXLib = GetGameInstance()->GetSubsystem<UEPFXPreloadLibrary>();
	FXLib->OnPreloadCompleted.AddDynamic(this, &AEPChessGameplayManager::StartGame);

	GetWorld()->GetTimerManager().SetTimer(TurnTimer, this, &AEPChessGameplayManager::StartGame, 1.0f, false);

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
		0.5f,
		false
	);
}

void AEPChessGameplayManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 유닛 활동 정지
	for (AActor* Unit : Units)
	{
		if (IsValid(Unit))
		{
			Unit->SetActorTickEnabled(false);
		}
	}

	// 위젯 제거
	if (GameUIInstance)
	{
		GameUIInstance->RemoveFromParent();
	}
}

// Called every frame
void AEPChessGameplayManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AEPChessGameplayManager::StartGame()
{
	GetWorld()->GetTimerManager().SetTimer(TurnTimer, this, &AEPChessGameplayManager::OnTurn, 3.0f, true);
}

void AEPChessGameplayManager::ClearGame(AActor* DeadKing)
{
	UE_LOG(LogTemp, Warning, TEXT("Game Clear! King %s is dead."), *DeadKing->GetName());

	for (ACUnit* Unit : GetUnit()) 
	{
		Unit->ClearGame();
	}

	bIsClear = true;

	for (int32 i = 1; i <= 9;i++)
	{
		for (int32 j = 1;j <= 9;j++)
		{
			SetGridWarning(FIntPoint(i, j), -GetGridWarning(FIntPoint(i, j)));
		}
	}

	// 턴 타이머 제거
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// 게임 종료 (시각적) 처리 - 블루프린트에서 정의
	PlayOpenDoor();
}

void AEPChessGameplayManager::OnTurn()
{
	Turn += 1;

	for (ACUnit* Unit : GetUnit())
	{
		AChessUnitController* Controller = Cast<AChessUnitController>(Unit->GetController());
		int32 NowTurn = Turn;
		FIntPoint BeforeMovePoint = Unit->GetXY();
		FIntPoint MovingPlace = FIntPoint(-1, -1);

		if (NowTurn % 10 == 0) {
			if (Unit->GetUnitTurnType() == (NowTurn / 10 + 2) % 3 + 1) {
				MovingPlace = Controller->MoveOut();
			}
			else if (Unit->GetUnitName() == FName("Pawn")) {
				BeforeMovePoint = Controller->Attack();
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
		else if (NowTurn % 10 == 8) {
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
				Unit->Warning();
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
		SetGridState(BeforeMovePoint, MovingPlace, Unit->GetUnitType());
	}

}

FIntPoint AEPChessGameplayManager::FindRandomMoveInPlace()
{
	FIntPoint TargetXY(0, 0);

	const int32 LastIndex = RandomPoint.Num() - 1;
	for (int32 i = 0; i <= LastIndex; i++)
	{
		int32 Index = FMath::RandRange(i, LastIndex);
		if (i != Index)
		{
			RandomPoint.Swap(i, Index);
		}
	}
	for (int32 i = 0; i <= LastIndex; i++)
	{
		if (GetGridState(RandomPoint[i]) == 0) {
			TargetXY = RandomPoint[i];
		}
	}

	return TargetXY;
}

void AEPChessGameplayManager::SetGridState(FIntPoint OldXY, FIntPoint NewXY, int32 UnitType)
{
	GridState[OldXY.X][OldXY.Y] = 0;
	if (OldXY.X == 0 || OldXY.X == 9 || OldXY.Y == 0 || OldXY.Y == 9) GridState[OldXY.X][OldXY.Y] = -1;
	if (NewXY.X >= 0 && NewXY.X < 10 && NewXY.Y >= 0 && NewXY.Y < 10)GridState[NewXY.X][NewXY.Y] = UnitType;
}

int32 AEPChessGameplayManager::GetGridState(FIntPoint NewXY)
{
	int32 XX = NewXY.X;
	int32 YY = NewXY.Y;
	if (XX > 0 && XX < 9 && YY > 0 && YY < 9) {
		return GridState[XX][YY];
	}
	return -1;
}

FVector AEPChessGameplayManager::GetGridVector(FIntPoint NewXY)
{
	return GridVector[NewXY.X][NewXY.Y];
}

FVector AEPChessGameplayManager::GetPlayerVector()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	APawn* PlayerPawn = PlayerController->GetPawn();
	return PlayerPawn->GetActorLocation();
}

void AEPChessGameplayManager::SetGridWarning(FIntPoint NewXY, int32 Val)
{
	if (!bIsClear || GridWarningState[NewXY.X][NewXY.Y] + Val == 0)
	{
		GridWarningState[NewXY.X][NewXY.Y] += Val;
		GridWarningTiles[NewXY.X * 10 + NewXY.Y]->SetActorHiddenInGame(GridWarningState[NewXY.X][NewXY.Y] == 0 ? true : false);
	}
}

void AEPChessGameplayManager::GamestateSetup()
{
	/////Chess Grid Vector init
	FString TargetActorName = TEXT("ChessBoard_GEN_VARIABLE_BP_ChessBoardBox_C_CAT_0");
	AActor* ChessBoardActor = nullptr;

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

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

	if (ChessBoardActor)
	{
		FVector TempVector = FVector(ChessBoardActor->GetActorLocation().X - (GridSize * 4.5f), ChessBoardActor->GetActorLocation().Y - (GridSize * 4.5f), ChessBoardActor->GetActorLocation().Z + 1.f);
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				GridVector[i][j] = FVector(TempVector.X + (GridSize * i), TempVector.Y + (GridSize * j), TempVector.Z);
				/*GridWarningTiles.Add(GetWorld()->SpawnActor<AActor>(WarningTileBP, GridVector[i][j], SpawnRotation, SpawnParams));
				GridWarningTiles[i * 10 + j]->SetActorHiddenInGame(true);*/
				AActor* NewTile = GetWorld()->SpawnActor<AActor>(WarningTileBP, GridVector[i][j], SpawnRotation, SpawnParams);
				if (NewTile)
				{
					NewTile->SetActorHiddenInGame(true);
					GridWarningTiles.Add(NewTile);
				}
				//---
				GridWarningState[i][j] = 0;
				if (i == 0 || i == 9 || j == 0 || j == 9) GridState[i][j] = -1;
				else GridState[i][j] = 0;
			}
		}
	}
	else
	{
		return;
	}


	/////Spawn

	if (KingBP)
	{
		ACKing* TempKing = GetWorld()->SpawnActor<ACKing>(KingBP, GridVector[8][4], SpawnRotation, SpawnParams);
		TempKing->SetOriginPoint(FIntPoint(8, 4));
		TempKing->OnKingDie.AddDynamic(this, &AEPChessGameplayManager::ClearGame);
		GridState[8][4] = 1;
		Units.Add(TempKing);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("King unit class is null -- edit setting please"));
	}

	if (QueenBP)
	{
		ACQueen* TempQueen = GetWorld()->SpawnActor<ACQueen>(QueenBP, GridVector[8][5], SpawnRotation, SpawnParams);
		TempQueen->SetOriginPoint(FIntPoint(8, 5));
		GridState[8][5] = 2;
		Units.Add(TempQueen);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Queen unit class is null -- edit setting please"));
	}

	int32 TempX = 9, TempY = 4, MovingGrid = 1;

	if (BishopBP)
	{
		for (int32 i = 0; i < BishopNum; i++) {
			ACBishop* TempBishop = GetWorld()->SpawnActor<ACBishop>(BishopBP, GridVector[TempX][TempY], SpawnRotation, SpawnParams);
			TempBishop->SetOriginPoint(FIntPoint(TempX, TempY));
			GridState[TempX][TempY] = 2;
			Units.Add(TempBishop);

			TempY += MovingGrid;
			MovingGrid *= (-1);
			MovingGrid > 0 ? MovingGrid += 1 : MovingGrid -= 1;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Bishop unit class is null -- edit setting please"));
	}

	if (KnightBP)
	{
		TempX = 4, TempY = 0, MovingGrid = 1;
		for (int32 i = 0; i < KnightNum; i++) {
			ACKnight* TempKnight = GetWorld()->SpawnActor<ACKnight>(KnightBP, GridVector[TempX][TempY], SpawnRotation, SpawnParams);
			TempKnight->SetOriginPoint(FIntPoint(TempX, TempY));
			GridState[TempX][TempY] = 2;
			Units.Add(TempKnight);

			TempX += MovingGrid;
			MovingGrid *= (-1);
			MovingGrid > 0 ? MovingGrid += 1 : MovingGrid -= 1;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Knight unit class is null -- edit setting please"));
	}

	if (RookBP)
	{
		TempX = 5, TempY = 9, MovingGrid = -1;
		for (int32 i = 0; i < RookNum; i++) {
			ACRook* TempRook = GetWorld()->SpawnActor<ACRook>(RookBP, GridVector[TempX][TempY], SpawnRotation, SpawnParams);
			TempRook->SetOriginPoint(FIntPoint(TempX, TempY));
			GridState[TempX][TempY] = 2;
			Units.Add(TempRook);

			TempX += MovingGrid;
			MovingGrid *= (-1);
			MovingGrid > 0 ? MovingGrid += 1 : MovingGrid -= 1;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Rook unit class is null -- edit setting please"));
	}

	if (PawnBP)
	{
		TempX = 0, TempY = 0;
		for (int32 i = 0; i < PawnNum; i++) {
			ACPawn* TempPawn = GetWorld()->SpawnActor<ACPawn>(PawnBP, GridVector[TempX][TempY], SpawnRotation, SpawnParams);
			TempPawn->SetActorHiddenInGame(true);
			TempPawn->SetActorEnableCollision(false);
			Units.Add(TempPawn);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn unit class is null -- edit setting please"));
	}
}
