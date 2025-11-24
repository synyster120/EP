// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AnimNotifies/EPAnimNotify_PickUp.h"
#include "Characters/EPPlayerCharacter.h"

void UEPAnimNotify_PickUp::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (AEPPlayerCharacter* Character = Cast<AEPPlayerCharacter>(MeshComp->GetOwner()))
    {
        Character->PickUp();
    }
}
