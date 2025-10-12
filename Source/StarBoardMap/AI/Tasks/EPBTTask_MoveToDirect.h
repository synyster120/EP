// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EPBTTask_MoveToDirect.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPBTTask_MoveToDirect : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
    UEPBTTask_MoveToDirect();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess) override;

    UPROPERTY(EditAnywhere, Category = "AI")
    FBlackboardKeySelector TargetKey;
};
