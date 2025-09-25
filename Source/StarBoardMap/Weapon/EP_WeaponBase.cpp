// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/EP_WeaponBase.h"
#include "Data/EPWeaponTypes.h"

void AEP_WeaponBase::SpawnWeapon(FName SpawnWeaponName)
{
	UEPWeaponTypes* WeaponData = LoadObject<UEPWeaponTypes>(nullptr, TEXT("/Game/AssetDynamic/Data/Weapon/BP_WeaponTypes.BP_WeaponTypes"));
	FWeaponInfo Data = WeaponData->GetWeaponInfoByName(FName(SpawnWeaponName));

	WeaponBP = GetWorld()->SpawnActor<AActor>(Data.WeaponBlueprint);
}

void AEP_WeaponBase::StartWithCharacter()
{
}

void AEP_WeaponBase::SetAttackDamage(int32 NewDamage)
{
}

void AEP_WeaponBase::OnPickedUp(AActor* Picker)
{
}

void AEP_WeaponBase::OnAttack(AActor* Enemy)
{
}
