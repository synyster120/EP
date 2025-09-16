// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Chess/BTTask_ChessUnitMoveToEnd.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_ChessUnitMoveToEnd::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;

    if (OwnerComp.GetBlackboardComponent()->GetValueAsBool("IsBigJump")) {
    }
    else {
        FVector TargetVector = OwnerComp.GetBlackboardComponent()->GetValueAsVector("TargetPoint");
        OwnerComp.GetBlackboardComponent()->SetValueAsVector("NowTargetPoint", TargetVector);

        float Speed = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("FallingSpeed");
        OwnerComp.GetBlackboardComponent()->SetValueAsFloat("NowMovingSpeed", Speed);
    }
    int32 NowState = OwnerComp.GetBlackboardComponent()->GetValueAsInt("NowState");
    OwnerComp.GetBlackboardComponent()->SetValueAsInt("NowState", NowState + 1);
    OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsMoving", true);

	return EBTNodeResult::Type();
}
