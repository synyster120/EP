// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillDataAsset.generated.h"

class USkillBase;
struct FSkillPhaseData;
/**
 * 
 */

 // 최종 스킬 데이터 (데이터 테이블 행)
USTRUCT(BlueprintType)
struct FSkillData
{
    GENERATED_BODY()

public:
    // 스킬명
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    FString SkillName = "None";

    // 쿨타임
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    float Cooldown; 

    // 스킬 단계 배열
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    TArray<FSkillPhaseData> SkillSteps;

    // 스킬 클래스 타입
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    TSoftClassPtr<USkillBase> SkillClass;
};

UCLASS()
class STARBOARDMAP_API USkillDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FSkillData SkillData;
};
