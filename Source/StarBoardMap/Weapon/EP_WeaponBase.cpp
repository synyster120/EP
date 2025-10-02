// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/EP_WeaponBase.h"

// Sets default values for this component's properties
UEP_WeaponBase::UEP_WeaponBase()
{
	
}


// Called when the game starts
void UEP_WeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

void UEP_WeaponBase::SpawnWeapon(FName SpawnWeaponName)
{
}

void UEP_WeaponBase::StartWithCharacter()
{
	//attach to character
}
