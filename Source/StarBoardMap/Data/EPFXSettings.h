// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EPFXSettings.generated.h"

class UEPFXData;

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "EP FX Settings"))
class STARBOARDMAP_API UEPFXSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, config, Category = "FX")
	TSoftObjectPtr<UEPFXData> DefaultFXData;
};
