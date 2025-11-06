// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Chess/BTTask_ChessUnitAttack.h"
#include "AIController.h"
#include "AI/ChessUnitController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_ChessUnitAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
    AChessUnitController* Controller = Cast<AChessUnitController>(ControlledPawn->GetController());

    Controller->MoveEnd();
    if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(FName("IsAttack"))) {
        Controller->Attack();
    }
    OwnerComp.GetBlackboardComponent()->SetValueAsInt(FName("NowState"), 0);

    return EBTNodeResult::Type();
}
