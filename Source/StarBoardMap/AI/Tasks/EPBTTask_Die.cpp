// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/EPBTTask_Die.h"
#include "AIController.h"
#include "Core/Interfaces/EPCombatEventInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

UEPBTTask_Die::UEPBTTask_Die()
{
	bNotifyTick = false;
    NodeName = TEXT("Die");
}

EBTNodeResult::Type UEPBTTask_Die::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
    AActor* OwnerActor = AIController ? AIController->GetPawn() : nullptr;
	if (!OwnerActor) return EBTNodeResult::Failed;
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if (BlackboardComp)
    {
        if (OwnerActor->Implements<UEPCombatEventInterface>()) // interface 확인
        {
            // 죽음 함수 호출
            IEPCombatEventInterface::Execute_HandleDeath(OwnerActor);

            BlackboardComp->SetValueAsBool(HasProcessedDeathKey.SelectedKeyName, true);

            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}
