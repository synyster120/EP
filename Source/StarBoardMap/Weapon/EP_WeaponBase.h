// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EP_WeaponBase.generated.h"

/*
***		WeaponBase 역할 : 무기의 공격력 추가 연산, 충돌 소켓 저장 및 반환 (외부에서 GetSocketLocation 사용)
*/

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
