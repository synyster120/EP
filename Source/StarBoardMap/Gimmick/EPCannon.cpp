// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/EPCannon.h"
#include "Components/EPSkillComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
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

FVector AEPCannon::GetMuzzleLocation() const
{
	return GetActorTransform().TransformPosition(TargetOffset);
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

	TArray<UStaticMeshComponent*> Comps;
	GetComponents<UStaticMeshComponent>(Comps);

	for (UStaticMeshComponent* C : Comps)
	{
		if (!IsValid(C)) continue;
		const FString NameStr = C->GetName();
		if (NameStr.StartsWith(TEXT("CannonFloor"))) 
			FloorComponent = C;
		else if (NameStr.StartsWith(TEXT("CannonBody")))
			BodyComponent = C;
	}

	TArray<USceneComponent*> Comps2;
	GetComponents<USceneComponent>(Comps2);

	for (USceneComponent* C : Comps2)
	{
		if (!IsValid(C)) continue;
		const FString NameStr = C->GetName();
		if (NameStr.StartsWith(TEXT("CannonBodyScene")))
			BodySceneComponent = C;
	}
}

// Called every frame
void AEPCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	//Floor
	if (IsTurnFloor) {
		FRotator TargetRot(0.f, FloorValue, 0.f);
		FRotator CurrentRot = FloorComponent->GetComponentRotation();


		FRotator NewRot = FMath::RInterpConstantTo(
			CurrentRot,
			TargetRot,
			DeltaTime,
			YawSpeedDegPerSec
		);


		if (NewRot.Equals(TargetRot, 1.f))
		{
			FloorComponent->SetWorldRotation(TargetRot);
			IsTurnFloor = false;
		}
		else {
			FloorComponent->SetWorldRotation(NewRot);
		}
	}

	//Body
	if(IsTurnBody){
		FRotator TargetRot(BodyValue-90.f, 0.f, 0.f);
		FRotator CurrentRot = BodySceneComponent->GetRelativeRotation();

		FRotator NewRot = FMath::RInterpConstantTo(
			CurrentRot,
			TargetRot,
			DeltaTime,
			YawSpeedDegPerSec
		);

		if (NewRot.Equals(TargetRot, 1.f))
		{
			BodySceneComponent->SetRelativeRotation(TargetRot);
			IsTurnBody = false;
		}
		else {
			BodySceneComponent->SetRelativeRotation(NewRot);
		}

		

	}
}

void AEPCannon::AimTarget(float NewFloorValue, float NewBodyValue)
{
	FloorValue = NewFloorValue;
	BodyValue = NewBodyValue;
	IsTurnFloor = IsTurnBody = true;
}

