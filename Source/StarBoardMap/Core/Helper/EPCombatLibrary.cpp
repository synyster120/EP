// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Helper/EPCombatLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

void UEPCombatLibrary::AbortPathAndStopMovement(AActor* Actor)
{
    if (!Actor) return;

    // Controller/PathFollowing 중단
    if (APawn* Pawn = Cast<APawn>(Actor))
    {
        if (AAIController* AIController = Cast<AAIController>(Pawn->GetController()))
        {
            // 남아있는 MoveTo/추격을 즉시 끊기
            AIController->StopMovement();
            if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
            {
                // 강제 스크립트 중단 플래그로 Abort
                PathFollowingComponent->AbortMove(*Actor, FPathFollowingResultFlags::ForcedScript);
            }
        }
    }

    // 현재 프레임 속도 제거 (재가속은 PathFollowing가 끊겨서 발생 안 함)
    if (ACharacter* C = Cast<ACharacter>(Actor))
    {
        if (UCharacterMovementComponent* CharacterMovement = C->GetCharacterMovement())
        {
            CharacterMovement->StopMovementImmediately();
            // 여기서는 DisableMovement까지는 하지 않음 (정지는 ‘락’ 대상 스킬만)
        }
    }
}
