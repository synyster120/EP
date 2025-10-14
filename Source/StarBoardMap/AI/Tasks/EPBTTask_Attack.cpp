// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/EPBTTask_Attack.h"
#include "AIController.h"
#include "Characters/EPEnemyCharacter.h"

UEPBTTask_Attack::UEPBTTask_Attack()
{
	bNotifyTick = false;
}

EBTNodeResult::Type UEPBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	AEPEnemyCharacter* OwnerEnemy = AIController ? Cast<AEPEnemyCharacter>(AIController->GetPawn()) : nullptr;
	if (!OwnerEnemy) return EBTNodeResult::Failed;

	OwnerEnemy->BaseAttack();

	return EBTNodeResult::Succeeded;
}
