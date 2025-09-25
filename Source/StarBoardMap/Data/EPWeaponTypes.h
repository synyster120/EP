// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EPWeaponTypes.generated.h"

USTRUCT(BlueprintType)
struct FWeaponInfo
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> WeaponBlueprint;
};

UCLASS()
class STARBOARDMAP_API UEPWeaponTypes : public UDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FWeaponInfo> WeaponList;

    const FWeaponInfo GetWeaponInfoByName(FName WeaponID) const
    {
        for (const FWeaponInfo& Info : WeaponList)
        {
            if (Info.WeaponName == WeaponID)
            {
                return Info;
            }
        }
        return FWeaponInfo();
    }
};
