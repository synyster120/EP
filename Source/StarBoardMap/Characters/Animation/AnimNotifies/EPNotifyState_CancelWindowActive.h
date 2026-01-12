// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "EPNotifyState_CancelWindowActive.generated.h"

/**
 *		SkillComponent에 skill 시전을 요청할 때, 
 *		다른 행동(콤보 연계, 스킬 캔슬 후 다른 행동)을 할 수 있는 상태로 세팅
 */

UCLASS()
class STARBOARDMAP_API UEPNotifyState_CancelWindowActive : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
