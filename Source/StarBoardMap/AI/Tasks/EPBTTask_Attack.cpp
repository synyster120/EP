// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/EPBTTask_Attack.h"
#include "AIController.h"
#include "Characters/EPEnemyCharacter.h"
#include "Components/EPSkillComponent.h"

UEPBTTask_Attack::UEPBTTask_Attack()
{
	bNotifyTick = false;
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UEPBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	MyOwnerComp = OwnerComp;
	AAIController* AIController = OwnerComp.GetAIOwner();
	AEPEnemyCharacter* OwnerEnemy = AIController ? Cast<AEPEnemyCharacter>(AIController->GetPawn()) : nullptr;
	if (!OwnerEnemy) return EBTNodeResult::Failed;

	BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return EBTNodeResult::Failed;

	// 공격 시전
	OwnerEnemy->BaseAttack();

	if (UEPSkillComponent* SkillComponent = OwnerEnemy->GetSkillComponent())
	{
		SkillComponent->OnSkillCooldownEnded.RemoveDynamic(this, &UEPBTTask_Attack::OnSkillReady);
		SkillComponent->OnSkillCooldownEnded.AddDynamic(this, &UEPBTTask_Attack::OnSkillReady);
		BlackboardComponent->SetValueAsBool(CanAttackKey.SelectedKeyName, false);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UEPBTTask_Attack::OnSkillReady(int32 SkillIndex, UObject* Instigator)
{
	if (!SkillIndex && !MyOwnerComp && !BlackboardComponent) return;

	// 스킬 가능 상태 update
	BlackboardComponent->SetValueAsBool(CanAttackKey.SelectedKeyName, true);
}
