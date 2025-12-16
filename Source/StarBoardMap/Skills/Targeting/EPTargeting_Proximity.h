// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/Targeting/EPTargetingStrategy.h"
#include "EPTargeting_Proximity.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPTargeting_Proximity : public UEPTargetingStrategy
{
	GENERATED_BODY()

public:
    // 탐색 반경 (에디터에서 수정 가능하도록 설정하거나, 데이터 에셋에서 가져와도 됨)
    UPROPERTY(EditAnywhere, Category = "Targeting")
    float SearchRadius = 1000.0f;

    // 아군/적군 구분을 위한 태그나 채널 설정이 필요할 수 있음
    UPROPERTY(EditAnywhere, Category = "Targeting")
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToDetect;

	virtual bool FindTarget(AActor* Caster, const FEPSkillPhaseData& PhaseData, FEPSkillTargetData& OutTargetData) override;
};
