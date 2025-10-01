// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/EPSkillTypes.h"
#include "EPObjectPoolTypes.generated.h"

/**
 *      Object Pooling 관련 타입 모음
 */

// 모든 풀링 가능한 객체가 공통으로 사용할 '표준 초기화 데이터' 구조체
USTRUCT(BlueprintType)
struct FEPPoolableObjectInitializer
{
    GENERATED_BODY()

    // 풀링을 요청한 Owner Actor
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> Owner;

    // 풀링 Object 를 초기화할 Data
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UDataAsset> Data; 

    // 타겟팅 정보
    UPROPERTY(BlueprintReadWrite)
    FEPSkillTargetData TargetData;

    // 투사체를 발사할 최종 속도
    UPROPERTY(BlueprintReadWrite)
    FVector LaunchVelocity;
};
