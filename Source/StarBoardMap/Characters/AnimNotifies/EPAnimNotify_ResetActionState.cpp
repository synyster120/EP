// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AnimNotifies/EPAnimNotify_ResetActionState.h"
#include "Characters/EPCharacterBase.h"

void UEPAnimNotify_ResetActionState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

    if (AEPCharacterBase* Character = Cast<AEPCharacterBase>(MeshComp->GetOwner()))
    {
        // 캐릭터의 상태를 Idle 로 변경
        Character->SetCurrentState(EEPCharacterState::Idle);
    }
}
