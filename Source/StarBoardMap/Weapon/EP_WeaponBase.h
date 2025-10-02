// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EP_WeaponBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARBOARDMAP_API UEP_WeaponBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEP_WeaponBase();
	
protected:
	FName WeaponName;
	int32 Damage;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SpawnWeapon(FName SpawnWeaponName);
	void StartWithCharacter();

public:	
	void SetAttackDamage(int32 NewDamage);
	void OnPickedUp(AActor* Picker);
	void OnPutDown();

	void OnAttack(AActor* Enemy);

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;
};
