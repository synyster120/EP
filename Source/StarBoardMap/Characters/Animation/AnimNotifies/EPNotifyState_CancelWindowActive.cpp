// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animation/AnimNotifies/EPNotifyState_CancelWindowActive.h"
#include "Components/EPSkillComponent.h"


void UEPNotifyState_CancelWindowActive::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    if (AActor* Owner = MeshComp->GetOwner())
    {
        // 컴포넌트를 찾아 스킬캔슬 가능 상태로 변경
        if (UEPSkillComponent* SkillComp = Owner->FindComponentByClass<UEPSkillComponent>())
        {
            SkillComp->SetbIsCancelWindowActive(true);
        }
    }
}

void UEPNotifyState_CancelWindowActive::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

    if (AActor* Owner = MeshComp->GetOwner())
    {
        if (UEPSkillComponent* SkillComp = Owner->FindComponentByClass<UEPSkillComponent>())
        {
            // 컴포넌트를 찾아 스킬캔슬 불가능 상태로 변경
            SkillComp->SetbIsCancelWindowActive(false);
        }
    }
}