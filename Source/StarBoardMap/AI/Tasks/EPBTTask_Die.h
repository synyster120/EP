// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EPBTTask_Die.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPBTTask_Die : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UEPBTTask_Die();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
