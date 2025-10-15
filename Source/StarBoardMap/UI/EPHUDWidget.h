// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EPHUDWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UImage* Image1;

    UPROPERTY(meta = (BindWidget))
    UImage* Image2;

    UPROPERTY(meta = (BindWidget))
    UImage* Image3;

    /*UFUNCTION(BlueprintNativeEvent, Category = "HUD")
    void UpdateHealthFloat(float NewHealth, float MaxHealth);*/
};
