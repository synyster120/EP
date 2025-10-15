// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EPMovementLockComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

void UEPMovementLockComponent::Acquire(FName Reason)
{
    if (!Reasons.Contains(Reason))
    {
        Reasons.Add(Reason);
        ++LockCount;
        if (LockCount == 1)
        {
            ApplyLock();
        }
    }
}

void UEPMovementLockComponent::Release(FName Reason)
{
    if (Reasons.Contains(Reason))
    {
        Reasons.Remove(Reason);
        LockCount = FMath::Max(0, LockCount - 1);
        if (LockCount == 0)
        {
            RemoveLock();
        }
    }
}

void UEPMovementLockComponent::ApplyLock()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    // 1) 경로/추격 즉시 중단
    if (APawn* Pawn = Cast<APawn>(Owner))
    {
        if (AAIController* AIController = Cast<AAIController>(Pawn->GetController()))
        {
            AIController->StopMovement();
            if (auto* PathFollowingComponent = AIController->GetPathFollowingComponent())
                PathFollowingComponent->AbortMove(*this, FPathFollowingResultFlags::ForcedScript);
        }
    }

    // 2) 이동 자체 비활성 (진짜 '고정' 보장)
    if (ACharacter* Character = Cast<ACharacter>(Owner))
    {
        if (auto* CharacterMovement = Character->GetCharacterMovement())
        {
            CharacterMovement->StopMovementImmediately();
            CharacterMovement->DisableMovement();               // MOVE_None 효과
        }

        // (옵션) 회전도 고정하고 싶다면 여기서 처리
        // C->bUseControllerRotationYaw = false;
        // if (auto* Ctrl = C->GetController()) Ctrl->SetControlRotation(C->GetActorRotation());
    }
}

void UEPMovementLockComponent::RemoveLock()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    if (ACharacter* Character = Cast<ACharacter>(Owner))
    {
        if (auto* CharacterMovement = Character->GetCharacterMovement())
        {
            // 기본 걷기 모드로 복귀 (프로젝트 룰에 맞게 조정)
            CharacterMovement->SetMovementMode(MOVE_Walking);
        }
    }
}