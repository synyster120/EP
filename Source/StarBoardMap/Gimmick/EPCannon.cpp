// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/EPCannon.h"
#include "Components/EPSkillComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEPCannon::AEPCannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkillComponent = CreateDefaultSubobject<UEPSkillComponent>(TEXT("SkillComponent"));

}

// Called when the game starts or when spawned
void AEPCannon::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultSkills.Num() > 0 && SkillComponent)
	{
		SkillComponent->InitializeSkills(DefaultSkills);
	}

	GetWorld()->GetTimerManager().SetTimer(AttackHandle,
		FTimerDelegate::CreateLambda([this]()
			{
				if (SkillComponent)
				{
					SkillComponent->ActivateSkill(0);
				}
			}),
		AttackRate,
		AttackLooping
	);
	TargetVector = GetActorLocation() + TargetWidget/5.f;

	TArray<UStaticMeshComponent*> Comps;
	GetComponents<UStaticMeshComponent>(Comps);

	for (UStaticMeshComponent* C : Comps)
	{
		if (!C) continue;
		const FName N = C->GetFName();
		UE_LOG(LogTemp, Warning, TEXT("Name is %s"), *N.ToString());
		if (N == TEXT("CannonFloor")) FloorComponent = C;
		else if (N == TEXT("CannonBody")) BodyComponent = C;
	}

	FTimerHandle FireTimerHandle;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&AEPCannon::AimTarget,
		5.f,
		true
	);
}

// Called every frame
void AEPCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FRotator LookAtRot =
		UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NowTargetVector);

	FRotator TargetRot(0.f, LookAtRot.Yaw, 0.f);
	FRotator CurrentRot = FloorComponent->GetComponentRotation();

	float YawSpeedDegPerSec = 180.f; // 튜닝 포인트

	FRotator NewRot = FMath::RInterpConstantTo(
		CurrentRot,
		TargetRot,
		DeltaTime,
		YawSpeedDegPerSec
	);

	FloorComponent->SetWorldRotation(NewRot);
}

void AEPCannon::AimTarget()
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return;

	if (FVector::DistSquared(Player->GetActorLocation(), TargetVector) <= FMath::Square(500.f)) {
		NowTargetVector = Player->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("HIHI ININ"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("HIHI Not ININ"));
		NowTargetVector = TargetVector;
	}

	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TempHandle,
		[this]()
		{
			SkillComponent->ActivateSkill(0);
		},
		1.0f,
		false
	);

	/*UE_LOG(LogTemp, Warning, TEXT("HIHI Player %f %f %f"), Player->GetActorLocation().X, Player->GetActorLocation().Y, Player->GetActorLocation().Z);
	UE_LOG(LogTemp, Warning, TEXT("HIHI NowTarget %f %f %f"), NowTargetVector.X, NowTargetVector.Y, NowTargetVector.Z);
	UE_LOG(LogTemp, Warning, TEXT("HIHI TargetWidget %f %f %f"), TargetWidget.X, TargetWidget.Y, TargetWidget.Z);
	UE_LOG(LogTemp, Warning, TEXT("HIHI TargetVector %f %f %f"), TargetVector.X, TargetVector.Y, TargetVector.Z);*/
}

