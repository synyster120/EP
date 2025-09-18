// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EPStatComponent.h"
#include "EPHealthBarStatComponent.generated.h"

struct FEPDamageInfo;

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPHealthBarStatComponent : public UEPStatComponent
{
	GENERATED_BODY()

public:
	UEPHealthBarStatComponent();
	// 체력 정보 반환 
	virtual FEPHealthInfo GetHealthInfo() const override;

	virtual void Initialize(const FEPBaseStat& BaseStatData) override;

	virtual void ApplyDamage(const FEPDamageInfo& DamageInfo) override;

	virtual bool IsDied() const override;
	virtual void CalculateAndApplyDamage(const FEPDamageInfo& DamageInfo) override;

protected:

protected:
	UPROPERTY(VisibleAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxHealthBar;

	UPROPERTY(VisibleAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentHealthBar;
};
