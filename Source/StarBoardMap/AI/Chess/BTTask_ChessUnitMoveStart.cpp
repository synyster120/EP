// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Chess/BTTask_ChessUnitMoveStart.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_ChessUnitMoveStart::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;

    float JumpHeight, Speed;
    FVector TargetVector = OwnerComp.GetBlackboardComponent()->GetValueAsVector("TargetPoint");
    if (OwnerComp.GetBlackboardComponent()->GetValueAsBool("IsBigJump")) {
        JumpHeight = 500;
        TargetVector.Z += JumpHeight;

        Speed = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("BigMovingSpeed");
    }
    else {
        JumpHeight = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("JumpHeight");
        TargetVector.Z += JumpHeight;
        
        Speed = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("MovingSpeed");
    }

    OwnerComp.GetBlackboardComponent()->SetValueAsVector("NowTargetPoint", TargetVector);
    OwnerComp.GetBlackboardComponent()->SetValueAsFloat("NowMovingSpeed", Speed);
    int32 NowState = OwnerComp.GetBlackboardComponent()->GetValueAsInt("NowState");
    OwnerComp.GetBlackboardComponent()->SetValueAsInt("NowState", NowState+1);
    OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsMoving", true);

    return EBTNodeResult::Type();
}
