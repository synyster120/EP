// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Subsystems/EPRespawnSubsystem.h"
#include "EPRespawnSubsystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEPRespawnSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    bIsRespawnPointSet = false;
}

void UEPRespawnSubsystem::SetRespawnPoint(const FTransform& NewTransform)
{
    CurrentRespawnTransform = NewTransform;
    bIsRespawnPointSet = true;

    // 체크포인트 로그
    UE_LOG(LogTemp, Log, TEXT("Respawn Point Updated: %s"), *NewTransform.GetLocation().ToString());
}

void UEPRespawnSubsystem::RespawnPlayer(AActor* PlayerActor)
{
    // 플레이어가 없거나 체크포인트가 설정되지 않았다면 중단
    if (!PlayerActor || !bIsRespawnPointSet)
    {
        UE_LOG(LogTemp, Warning, TEXT("Respawn Failed: Invalid Actor or Spawn Point not set."));
        return;
    }

    ACharacter* PlayerCharacter = Cast<ACharacter>(PlayerActor);
    if (PlayerCharacter)
    {
        // 물리 가속도 완전 초기화 (떨어지던 관성 제거)
        if (UCharacterMovementComponent* MovementComp = PlayerCharacter->GetCharacterMovement())
        {
            MovementComp->StopMovementImmediately();
            MovementComp->Velocity = FVector::ZeroVector;
        }

        // 텔레포트 (안전하게 이동하며 물리 상태 업데이트)
        PlayerCharacter->SetActorLocationAndRotation(
            CurrentRespawnTransform.GetLocation(),
            CurrentRespawnTransform.GetRotation(),
            false,
            nullptr,
            ETeleportType::TeleportPhysics
        );
        
        // 리스폰 성공 이벤트 방송 (플레이어 카메라 fade in)
        OnPlayerRespawned.Broadcast();
    }
}
