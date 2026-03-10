// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EPRespawnSubsystem.generated.h"

/**
 * 
 */
 // 리스폰 완료 방송용 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerRespawnedSignature);

UCLASS()
class STARBOARDMAP_API UEPRespawnSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // 체크포인트가 호출할 스폰 위치 저장 함수
    UFUNCTION(BlueprintCallable, Category = "RespawnSystem")
    void SetRespawnPoint(const FTransform& NewTransform);

    // 낙사 구역(Trigger Volume)이 호출할 리스폰 실행 함수
    UFUNCTION(BlueprintCallable, Category = "RespawnSystem")
    void RespawnPlayer(AActor* PlayerActor);

    // 블루프린트에서 바인딩할 수 있는 리스폰 이벤트
    UPROPERTY(BlueprintAssignable, Category = "RespawnSystem|Events")
    FOnPlayerRespawnedSignature OnPlayerRespawned;

private:
    // 현재 저장된 리스폰 트랜스폼 (위치, 회전, 스케일)
    UPROPERTY()
    FTransform CurrentRespawnTransform;

    // 체크포인트를 한 번이라도 밟았는지 확인하는 안전장치
    UPROPERTY()
    bool bIsRespawnPointSet = false;
};
