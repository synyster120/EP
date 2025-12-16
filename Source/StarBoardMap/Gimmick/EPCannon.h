// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interfaces/EPCombatQueryInterface.h"
#include "EPCannon.generated.h"

class UEPSkillComponent;
class UEPSkillDataAsset;
class UStaticMeshComponent;
class UEPTargetingStrategy;

UCLASS()
class STARBOARDMAP_API AEPCannon : public AActor, public IEPCombatQueryInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEPCannon();

	UPROPERTY(EditAnywhere, Category = "Skill | Rate")
	float AttackRate = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Skill | Looping")
	bool AttackLooping = true;


	// interface
	virtual FVector GetMuzzleLocation() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEPSkillComponent> SkillComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TArray<TSoftObjectPtr<UEPSkillDataAsset>> DefaultSkills;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* FloorComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BodyComponent;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cannon", meta = (MakeEditWidget = true))
	FVector TargetOffset;

	FVector NowTargetVector;
	FVector TargetVector;

	float FloorValue = 0;
	float BodyValue = 0;
	

	UPROPERTY()
	FTimerHandle AttackHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AimTarget();

};
