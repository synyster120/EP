// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/EPCannon.h"
#include "Components/EPSkillComponent.h"

// Sets default values
AEPCannon::AEPCannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkillComponent = CreateDefaultSubobject<UEPSkillComponent>(TEXT("SkillComponent"));

}

FVector AEPCannon::GetMuzzleLocation() const
{
	return GetActorTransform().TransformPosition(GuardOffset);
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
}

// Called every frame
void AEPCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

