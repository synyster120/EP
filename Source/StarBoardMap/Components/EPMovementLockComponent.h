// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EPMovementLockComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARBOARDMAP_API UEPMovementLockComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "MovementLock")
    void Acquire(FName Reason);

    UFUNCTION(BlueprintCallable, Category = "MovementLock")
    void Release(FName Reason);

    UFUNCTION(BlueprintCallable, Category = "MovementLock")
    bool IsLocked() const { return LockCount > 0; }

private:
    int32 LockCount = 0;
    TSet<FName> Reasons;

    void ApplyLock();
    void RemoveLock();
};
