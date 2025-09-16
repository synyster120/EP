// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ChessUnitData.generated.h"

USTRUCT(BlueprintType)
struct FUnitData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float JumpHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 AttackProbability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MovingSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FallingSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BigMovingSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BigFallingSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StopTime1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StopTime2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FIntPoint> AvaliablePoint;
};

UCLASS()
class STARBOARDMAP_API UChessUnitData : public UDataAsset
{
	GENERATED_BODY()
	

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FUnitData> UnitDataArray;
    
public:
    UFUNCTION(BlueprintCallable)
    FUnitData FindUnitDataByName(FName TargetName) const
    {
        for (const FUnitData& Data : UnitDataArray)
        {
            if (Data.Name == TargetName)
            {
                return Data;
            }
        }
        return FUnitData();
    }
};
