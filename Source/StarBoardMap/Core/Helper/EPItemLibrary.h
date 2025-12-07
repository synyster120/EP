// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Items/EPDroppedItem.h"
#include "EPItemLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPItemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    /**
     * 월드에 아이템(껍데기)을 스폰하고 데이터를 주입하는 공용 함수
     * @param WorldContextObject : GetWorld()를 호출하기 위한 객체 (보통 this)
     * @param DroppedItemClass : 스폰할 껍데기 클래스 (BP_DroppedItem)
     * @param OriginalItemClass : 내용물 데이터 (BP_Potion 등)
     * @param SpawnLocation : 스폰될 위치
     */
    UFUNCTION(BlueprintCallable, Category = "Loot")
    static AEPDroppedItem* SpawnDroppedItem(
        UObject* WorldContextObject,
        TSubclassOf<AEPDroppedItem> DroppedItemClass,
        TSubclassOf<AActor> OriginalItemClass,
        FVector SpawnLocation,
        int32 Quantity = 1
    );
};
