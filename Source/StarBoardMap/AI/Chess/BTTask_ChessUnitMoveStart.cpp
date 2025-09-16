// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Chess/BTTask_ChessUnitMoveStart.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_ChessUnitMoveStart::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
    //UE_LOG(LogTemp, Warning, TEXT("INStart"));
    if (OwnerComp.GetBlackboardComponent()->GetValueAsBool("IsBigJump")) {
    }
    else {
        float JumpHeight = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("JumpHeight");
        FVector TargetVector = OwnerComp.GetBlackboardComponent()->GetValueAsVector("TargetPoint");
        TargetVector.Z += JumpHeight;
        OwnerComp.GetBlackboardComponent()->SetValueAsVector("NowTargetPoint", TargetVector);
        
        float Speed = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("MovingSpeed");
        OwnerComp.GetBlackboardComponent()->SetValueAsFloat("NowMovingSpeed", Speed);
    }
    int32 NowState = OwnerComp.GetBlackboardComponent()->GetValueAsInt("NowState");
    OwnerComp.GetBlackboardComponent()->SetValueAsInt("NowState", NowState+1);
    OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsMoving", true);

    return EBTNodeResult::Type();
}
