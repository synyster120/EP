// Fill out your copyright notice in the Description page of Project Settings.



#include "AI/Chess/BTTask_ChessUnitMove.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_ChessUnitMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	//UE_LOG(LogTemp, Warning, TEXT("INMOVE"));
	FVector TargetPoint = OwnerComp.GetBlackboardComponent()->GetValueAsVector("NowTargetPoint");
	float Speed = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("NowMovingSpeed");

	ControlledPawn->SetActorLocation(
		FMath::VInterpConstantTo(
			ControlledPawn->GetActorLocation(), 
			TargetPoint,
			GetWorld()->GetDeltaSeconds(), 
			Speed));

	FVector Current = ControlledPawn->GetActorLocation();
	float Distance = FVector::Dist(Current, TargetPoint);
	if (Distance <= 3.0f)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsMoving", false);
	}

	return EBTNodeResult::Type();
}
