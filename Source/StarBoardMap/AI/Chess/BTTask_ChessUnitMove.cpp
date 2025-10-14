// Fill out your copyright notice in the Description page of Project Settings.



#include "AI/Chess/BTTask_ChessUnitMove.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_ChessUnitMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bNotifyTick = true;
	return EBTNodeResult::InProgress;
}

void UBTTask_ChessUnitMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	ACharacter* Character = Cast<ACharacter>(ControlledPawn);

	FVector TargetPoint = OwnerComp.GetBlackboardComponent()->GetValueAsVector("NowTargetPoint");
	TargetPoint.Z += 1;
	float Speed = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("NowMovingSpeed");

	FVector Current = Character->GetActorLocation();
	float Distance = FVector::Dist(Current, TargetPoint);
	float MovementDelta = Speed * DeltaSeconds;

	if (Distance <= MovementDelta)
	{
		Character->SetActorLocation(TargetPoint); // 정확히 위치 고정
		Character->GetCharacterMovement()->StopMovementImmediately();
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsMoving", false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	else {
		FVector Direction = (TargetPoint - Character->GetActorLocation()).GetSafeNormal();
		Character->AddMovementInput(Direction, Speed);
	}
}
