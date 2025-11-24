// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EP_WeaponBase.generated.h"

UCLASS()
class STARBOARDMAP_API AEP_WeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEP_WeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//void SpawnWeapon(FName SpawnWeaponName);

public:
	UPROPERTY(EditAnywhere)
	FName Name;

	void AttachToCharacter();
	void DetachFromCharacter();
};
