// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/EPSkillBase.h"
#include "Characters/EPCharacterBase.h"
#include "EPSkill_ExplodeSelf.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillFinishedDelegate);

class UNiagaraSystem;
class USoundBase;
/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPSkill_ExplodeSelf : public UEPSkillBase
{
	GENERATED_BODY()

protected:
    // 스킬 Windup/지속시간 타이머
    FTimerHandle SkillTimerHandle;
    
    // 이 스킬이 재생할 몽타주 (Player/AI 공용)
    /*UPROPERTY(EditDefaultsOnly, Category = "Skill")
    UAnimMontage* MontageToPlay;*/

    // 타이머가 만료됐을 때 호출되는 내부 함수
    void OnSkillEffectFinished();

    // 이 스킬을 사용하는 Caster (캐릭터)
    UPROPERTY()
    AEPCharacterBase* OwnerCaster;

    FVector ExplosionLocation;
    float Damage;
    float DamageRadius;

    UNiagaraSystem* Effect;
    USoundBase* Sound;
    const FName Locktext = TEXT("Skill");

public:
	// "나 끝났어!"라고 외부에 알릴 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnSkillFinishedDelegate OnSkillFinishedDelegate;

	virtual void Activate(ACharacter* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex) override;

	// 강제 중단 (AI의 BT Task가 'Abort'시 호출. Player도 피격 시 호출 가능)
	virtual void CancelSkillActivation();

};
