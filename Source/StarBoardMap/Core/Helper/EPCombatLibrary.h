// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EPCombatLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPCombatLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AI|Movement")
	static void AbortPathAndStopMovement(AActor* Actor);
};
