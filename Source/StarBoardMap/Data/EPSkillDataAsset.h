// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/EPSkillTypes.h"
#include "EPSkillDataAsset.generated.h"

class UEPSkillBase;

/**
 * 
 */

 // 최종 스킬 데이터 (데이터 테이블 행)
USTRUCT(BlueprintType)
struct FEPSkillData
{
    GENERATED_BODY()

public:
    // 스킬명
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    FString SkillName = "None";

    // 쿨타임
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    float Cooldown; 

    // 현재 스킬에서만 사용할 로컬 스킬 단계 배열
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    TArray<FEPSkillPhaseData> LocalSkillPhases;

    // 최종적인 콤보 순서 (레시피) - 외부 스킬 단계 참조 포함 (DT_SkillPhases)
    UPROPERTY(EditAnywhere)
    TArray<FEPComboStep> ComboSequence;

    // 스킬 클래스 타입
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    TSoftClassPtr<UEPSkillBase> SkillClass;
};

UCLASS()
class STARBOARDMAP_API UEPSkillDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FEPSkillData SkillData;
};
