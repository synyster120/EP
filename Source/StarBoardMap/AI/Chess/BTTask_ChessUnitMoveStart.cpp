// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Chess/BTTask_ChessUnitMoveStart.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/ChessUnitController.h"

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

    AChessUnitController* Controller = Cast<AChessUnitController>(ControlledPawn->GetController());
    OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttack", false);
    int32 HIHI = FMath::RandRange(0, 100);
    if (OwnerComp.GetBlackboardComponent()->GetValueAsName("Name") == FName("King")) {
        if (FMath::Abs(TargetVector.X - ControlledPawn->GetActorLocation().X) + FMath::Abs(TargetVector.Y - ControlledPawn->GetActorLocation().Y) < 15.f) {
            OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttack", true);
            Controller->Warning();
        }
    }
    else if(!OwnerComp.GetBlackboardComponent()->GetValueAsBool("IsBigJump") && HIHI < OwnerComp.GetBlackboardComponent()->GetValueAsInt(FName("AttackProbability"))) {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttack", true);
        Controller->Warning();
    }

    OwnerComp.GetBlackboardComponent()->SetValueAsVector("NowTargetPoint", TargetVector);
    OwnerComp.GetBlackboardComponent()->SetValueAsFloat("NowMovingSpeed", Speed);
    int32 NowState = OwnerComp.GetBlackboardComponent()->GetValueAsInt("NowState");
    OwnerComp.GetBlackboardComponent()->SetValueAsInt("NowState", NowState+1);
    OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsMoving", true);

    return EBTNodeResult::Type();
}
