// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EP_WeaponBase.generated.h"

UCLASS()
class STARBOARDMAP_API AEP_WeaponBase : public AActor
{
	GENERATED_BODY()

protected:
	FName WeaponName;
	int32 Damage;

	UPROPERTY()
	AActor* WeaponBP;

public:
	void SpawnWeapon(FName SpawnWeaponName);
	void StartWithCharacter();

	void SetAttackDamage(int32 NewDamage);
	void OnPickedUp(AActor* Picker);
	void OnAttack(AActor* Enemy);
};
