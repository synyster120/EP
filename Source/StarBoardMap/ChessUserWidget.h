// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/EPPlayerCharacter.h"
#include "ChessUserWidget.generated.h"

class UImage;
class UProgressBar;
class ACKing;
class AEPPlayerCharacter;
/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UChessUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    /*UPROPERTY(meta = (BindWidget))
    UImage* Image1;

    UPROPERTY(meta = (BindWidget))
    UImage* Image2;

    UPROPERTY(meta = (BindWidget))
    UImage* Image3;

    UPROPERTY()
    UImage* Image[3];*/

    UPROPERTY(meta = (BindWidget))
    UProgressBar* ProgressBar;

    virtual void NativeConstruct() override;

    AEPPlayerCharacter* Player;
    ACKing* King;

    UFUNCTION() void OnPlayerHealthChange();
    UFUNCTION()
    void OnHealthChange();
};
