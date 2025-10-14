// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/EPBTTask_ClearBlackboardValue.h"
#include "BehaviorTree/BlackboardComponent.h"

UEPBTTask_ClearBlackboardValue::UEPBTTask_ClearBlackboardValue()
{
	bNotifyTick = false;
}

EBTNodeResult::Type UEPBTTask_ClearBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(ClearValueIsBoolKey.SelectedKeyName, false);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
