// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/EPSkillBase.h"
#include "EPSkill_AreaAttack.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMontageEnded);

class UEPMovementLockComponent;
/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPSkill_AreaAttack : public UEPSkillBase
{
	GENERATED_BODY()

public:
	virtual void Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex) override;
	
	UFUNCTION()
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGameplayTag ActionTag);

	// 테스트용
	virtual void BeginDestroy() override;

protected:
	const FName Locktext = TEXT("Skill");
	UEPMovementLockComponent* MovementLock;
};
