// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EPStatComponent.h"
#include "EPHealthBlockStatComponent.generated.h"

struct FEPDamageInfo;
struct FEPHealthInfo;
struct FEPBaseStat;

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPHealthBlockStatComponent : public UEPStatComponent
{
	GENERATED_BODY()
	
public:
	UEPHealthBlockStatComponent();
	// 체력 정보 반환 
	virtual FEPHealthInfo GetHealthInfo() const override;

	virtual void Initialize(const FEPBaseStat& BaseStatData) override;

	virtual void ApplyDamage(const FEPDamageInfo& DamageInfo) override;

	virtual bool IsDied() const override;

	virtual void CalculateAndApplyDamage(const FEPDamageInfo& DamageInfo) override;

protected:


protected:
	UPROPERTY(EditDefaultsOnly)
	int32 MaxHealthBlocks = 5;

	UPROPERTY(VisibleAnywhere)
	int32 CurrentHealthBlocks;
};
