// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/EP_WeaponBase.h"
#include "Data/EPWeaponTypes.h"

AEP_WeaponBase::AEP_WeaponBase()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
}

void AEP_WeaponBase::SpawnWeapon(FName SpawnWeaponName)
{
	UEPWeaponTypes* WeaponData = LoadObject<UEPWeaponTypes>(nullptr, TEXT("/Game/AssetDynamic/Data/Weapon/BP_WeaponTypes.BP_WeaponTypes"));
	FWeaponInfo Data = WeaponData->GetWeaponInfoByName(FName(SpawnWeaponName));

	WeaponBP = GetWorld()->SpawnActor<AActor>(Data.WeaponBlueprint);
	WeaponBP->SetActorRelativeLocation(FVector(-1.f, 0.f, -2.f));
	WeaponBP->SetActorRelativeRotation(FRotator(-83.f, 0, 10.f));
	WeaponBP->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
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
