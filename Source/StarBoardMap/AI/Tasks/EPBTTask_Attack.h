// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EPBTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPBTTask_Attack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CanAttackKey;

private:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> MyOwnerComp;

	UBlackboardComponent* BlackboardComponent;
	AAIController* AIController;
protected:
	UEPBTTask_Attack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UFUNCTION()
	void OnSkillReady(int32 SkillIndex, UObject* Instigator);
	UFUNCTION()
	void OnMovementLockEnded();
};
