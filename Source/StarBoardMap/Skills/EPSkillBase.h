// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/Interfaces/EPSkillCastInterface.h" // skill is Movement lock
#include "EPSkillBase.generated.h"

class UEPSkillDataAsset;
struct FEPSkillTargetData;
struct FEPSkillPhaseData;

/**
 *		Skill 클래스의 베이스가 되는 클래스
 */
UCLASS()
class STARBOARDMAP_API UEPSkillBase : public UObject, public IEPSkillCastInterface
{
	GENERATED_BODY()
	
public:
	// interface (IEPSkillCastInterface)
	virtual bool RequiresMovementLock(int32 CurrentPhaseDataIndex) const override;
	virtual float GetWindupSeconds(int32 CurrentPhaseDataIndex) const override;


	void Initialize(UEPSkillDataAsset* NewSkillDataAsset);

	virtual void Activate(ACharacter* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex);


	inline UEPSkillDataAsset* GetSkillData() const { return SkillDataAsset; };
	FName GetSkillID() const;
	FEPSkillPhaseData* GetPhaseData(int32 CurrentPhaseDataIndex) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UEPSkillDataAsset> SkillDataAsset;
};
