// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/EPBTTask_Attack.h"
#include "AIController.h"
#include "AI/EPEnemyAIController.h"
#include "Characters/EPEnemyCharacter.h"
#include "Components/EPSkillComponent.h"
#include "Core/Helper/EPCombatLibrary.h"


UEPBTTask_Attack::UEPBTTask_Attack()
{
	bNotifyTick = false;
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UEPBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	MyOwnerComp = OwnerComp;
	AIController = OwnerComp.GetAIOwner();
	AEPEnemyCharacter* OwnerEnemy = AIController ? Cast<AEPEnemyCharacter>(AIController->GetPawn()) : nullptr;
	if (!OwnerEnemy) return EBTNodeResult::Failed;

	BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return EBTNodeResult::Failed;

	if (UEPSkillComponent* SkillComponent = OwnerEnemy->GetSkillComponent())
	{
		// 이동(경로) 중단
		UEPCombatLibrary::AbortPathAndStopMovement(OwnerEnemy);

		// 공격 시전
		OwnerEnemy->BaseAttack();

		SkillComponent->OnSkillCooldownEnded.RemoveDynamic(this, &UEPBTTask_Attack::OnSkillReady); // 초기화
		SkillComponent->OnSkillCooldownEnded.AddDynamic(this, &UEPBTTask_Attack::OnSkillReady); // 바인딩
		SkillComponent->OnMovementLockEnded.RemoveDynamic(this, &UEPBTTask_Attack::OnMovementLockEnded); // 초기화
		SkillComponent->OnMovementLockEnded.AddDynamic(this, &UEPBTTask_Attack::OnMovementLockEnded); // 바인딩
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

void UEPBTTask_Attack::OnMovementLockEnded()
{
	if (AEPEnemyAIController* EnemyAIConteroller = Cast<AEPEnemyAIController>(AIController))
	{
		EnemyAIConteroller->NotifyIsWindupUpdate();
	}
}
