// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/Interfaces/EPSkillCastInterface.h" // skill is Movement lock
#include "Core/EPGameplayTags.h"
#include "EPSkillBase.generated.h"

class UEPSkillDataAsset;
struct FEPSkillTargetData;
struct FEPSkillPhaseData;

// 스킬이 끝났음을 알리는 내부 델리게이트 (외부 노출 X, Component 전달용)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillFinishedNative, UEPSkillBase* /*SkillInstance*/);
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
	void BeginDestroy() override;

	virtual void Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex);
	void CancelSkillActivation();
	UFUNCTION()
	void HandleSkillEnded(FGameplayTag EndedTag);

	// data
	inline UEPSkillDataAsset* GetSkillData() const { return SkillDataAsset; };
	FName GetSkillID() const;
	FEPSkillPhaseData* GetPhaseData(int32 CurrentPhaseDataIndex) const;

	// 델리게이트
	FOnSkillFinishedNative OnSkillFinishedNative;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UEPSkillDataAsset> SkillDataAsset;

	FGameplayTag SkillPhaseDataMontageTag;
};
