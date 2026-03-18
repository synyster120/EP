// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CUnit.h"
#include "UObject/ConstructorHelpers.h"
#include "AI/ChessUnitController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

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

	UCapsuleComponent* RootCapsule = GetCapsuleComponent();
	if (!RootCapsule)
	{
		UE_LOG(LogTemp, Error, TEXT("RootCapsule is NULL"));
		return;
	}
	UStaticMeshComponent* MeshComp = nullptr;
	{
		TArray<USceneComponent*> RootChildren;
		RootCapsule->GetChildrenComponents(true, RootChildren);

		for (USceneComponent* Child : RootChildren)
		{
			MeshComp = Cast<UStaticMeshComponent>(Child);
			if (MeshComp)
				break;
		}
	}
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Error, TEXT("StaticMeshComponent NOT found under RootCapsule"));
		return;
	}
	UCapsuleComponent* InnerCapsule = nullptr;
	{
		TArray<USceneComponent*> MeshChildren;
		MeshComp->GetChildrenComponents(true, MeshChildren);

		for (USceneComponent* Child : MeshChildren)
		{
			InnerCapsule = Cast<UCapsuleComponent>(Child);
			if (InnerCapsule)
				break;
		}
	}
	if (!InnerCapsule)
	{
		UE_LOG(LogTemp, Error, TEXT("CapsuleComponent NOT found under StaticMesh"));
		return;
	}
	CapsuleComp = InnerCapsule;
	//CapsuleComp = Cast<UCapsuleComponent>(GetCapsuleComponent()->GetChildComponent(2)->GetChildComponent(0));


	if (CapsuleComp)
	{
		CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ACUnit::OnCapsuleOverlap);
	}
}
void ACUnit::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ACUnit::ClearGame()
{
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACUnit::CollisionHitEnable()
{
	bIsCollisionHitEnable = true;
}

void ACUnit::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) && OtherComp->GetName().StartsWith(TEXT("Capsule")))
	{
		ACharacter* OtherChar = Cast<ACharacter>(OtherActor);
		FVector Dir = (OtherChar->GetActorLocation() - GetActorLocation());
		Dir.Z = 0;
		Dir.Normalize();

		OtherChar->AddActorWorldOffset(Dir * 50.f, true);

		if (bIsCollisionHitEnable) {
			bIsCollisionHitEnable = false;
			UGameplayStatics::ApplyDamage(OtherActor, 20.f, GetController(), this, UDamageType::StaticClass());
			FTimerHandle HitTimer;
			GetWorld()->GetTimerManager().SetTimer(HitTimer, this, &ACUnit::CollisionHitEnable, 1.0f, false);
		}
	}
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

void ACUnit::SetMovingTurn(bool TurnState)
{
	MovingTurn = TurnState;
}

void ACUnit::SetOriginPoint(FIntPoint NewOriginPoint)
{
	OriginPoint = NewOriginPoint;
	SetXY(NewOriginPoint);
}

bool ACUnit::IsOnBoard(FIntPoint XY)
{
	if (XY.X > 0 && XY.X < 9 && XY.Y > 0 && XY.Y < 9) return true;
	return false;
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

FIntPoint ACUnit::FindMove()
{
	FIntPoint TargetXY = NowXY;
	AChessUnitController* MyController = Cast<AChessUnitController>(GetController());

	const int32 LastIndex = AvaliablePoint.Num() - 1;
	for (int32 j = 0;j < 3;j++) {
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				AvaliablePoint.Swap(i, Index);
			}
		}
	}

	for (const FIntPoint& Offset : AvaliablePoint)
	{
		FIntPoint NewXY = NowXY + Offset;

		if (MyController->GetGridState(NewXY) == 0)
		{
			TargetXY = NewXY;
			break;
		}
	}

	return TargetXY;
}

void ACUnit::Warning()
{
}
