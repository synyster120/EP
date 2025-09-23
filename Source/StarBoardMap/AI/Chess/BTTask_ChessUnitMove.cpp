// Fill out your copyright notice in the Description page of Project Settings.



#include "AI/Chess/BTTask_ChessUnitMove.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_ChessUnitMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bNotifyTick = true;
	return EBTNodeResult::InProgress;
}

void UBTTask_ChessUnitMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;

	FVector TargetPoint = OwnerComp.GetBlackboardComponent()->GetValueAsVector("NowTargetPoint");
	float Speed = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("NowMovingSpeed");

	ControlledPawn->SetActorLocation(
		FMath::VInterpConstantTo(
			ControlledPawn->GetActorLocation(),
			TargetPoint,
			DeltaSeconds,
			Speed));

	FVector Current = ControlledPawn->GetActorLocation();
	float Distance = FVector::Dist(Current, TargetPoint);
	if (Distance <= 3.0f)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsMoving", false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
