// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//#include "Data/EPSkillDataAsset.h"
#include "EPSkillBase.generated.h"

class UEPSkillDataAsset;
struct FEPSkillTargetData;
struct FEPSkillPhaseData;

/**
 *		Skill 클래스의 베이스가 되는 클래스
 */
UCLASS()
class STARBOARDMAP_API UEPSkillBase : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(UEPSkillDataAsset* NewSkillDataAsset);

	virtual void Activate(ACharacter* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex);


	inline UEPSkillDataAsset* GetSkillData() const { return SkillDataAsset; };
	FName GetSkillID() const;
	FEPSkillPhaseData GetPhaseData(int32 CurrentPhaseDataIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UEPSkillDataAsset> SkillDataAsset;
};
