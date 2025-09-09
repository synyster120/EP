// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
//#include "Path/To/Your/AItemActor.h"
#include "ItemTypes.generated.h"

//class AItemActor;

/**
 * 
 */

USTRUCT(BlueprintType)
struct FDropItemInfo
{
    GENERATED_BODY()

public:
    //UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    //TSoftClassPtr<AItemActor> ItemClass; // 드랍할 아이템(BP)

    // 드랍 확률
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float DropChance; 

    // 최소 드랍 개수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 MinQuantity; 

    // 최대 드랍 개수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 MaxQuantity;
};

USTRUCT(BlueprintType)
struct FDropTable : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TArray<FDropItemInfo> DropItems;

};