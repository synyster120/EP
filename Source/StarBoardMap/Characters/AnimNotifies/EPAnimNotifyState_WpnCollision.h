// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Data/EPSkillTypes.h"
#include "EPAnimNotifyState_WpnCollision.generated.h"

class EPSkillRangeShape;

UCLASS()
class STARBOARDMAP_API UEPAnimNotifyState_WpnCollision : public UAnimNotifyState
{
	GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 AttackPhase = 1;

    /*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    FEPSkillPhaseData SkillPhaseData;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    UEPSkillPhaseDataAsset* SkillDataAsset;*/

    UPROPERTY()
    TSet<AActor*> HitEnemies;

    FEPSkillRangeData SkillRangeData;
    EEPSkillRangeShape ShapeType;
    FVector Dimensions;

    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
    void DoAttackTrace(USkeletalMeshComponent* MeshComp);
};
