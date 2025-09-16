// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EPSkillBase.generated.h"

class UEPSkillDataAsset;
struct FEPSkillTargetData;

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPSkillBase : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(UEPSkillDataAsset* NewSkillDataAsset);

	inline FName GetSkillID() { return FName("AssetID"); };

	void Activate(const FEPSkillTargetData& NewTargetData);

	inline UEPSkillDataAsset* GetSkillData() { return tempSkillData; };

protected:
	UEPSkillDataAsset* tempSkillData;
};
