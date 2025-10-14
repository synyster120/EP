// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/EPBTTask_RotateToTarget.h"
#include "AIController.h"
#include "Characters/EPCombatCharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UEPBTTask_RotateToTarget::UEPBTTask_RotateToTarget()
{
	bNotifyTick = true;
    NodeName = TEXT("Rotate To Target");
}

EBTNodeResult::Type UEPBTTask_RotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    AEPCombatCharacterBase* Enemy = Cast<AEPCombatCharacterBase>(AIController->GetPawn());
    AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
    /*UE_LOG(LogTemp, Warning, TEXT("rotate to target task is play || location == x : %f, y :  %f, z : %f")
        , Target->GetTargetLocation().X, Target->GetTargetLocation().Y, Target->GetTargetLocation().Z);*/

    if (!Enemy || !Target) return EBTNodeResult::Failed;

    return EBTNodeResult::InProgress;
}

void UEPBTTask_RotateToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    AEPCombatCharacterBase* Enemy = AIController ? Cast<AEPCombatCharacterBase>(AIController->GetPawn()) : nullptr;
    AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));

    if (!Enemy || !Target)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    FVector ToTarget = (Target->GetActorLocation() - Enemy->GetActorLocation());
    ToTarget.Z = 0.0f;
    if (ToTarget.IsNearlyZero(1e-3f))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    float InterpSpeed = 5.0f; // 회전 속도

    FRotator TargetRot = ToTarget.GetSafeNormal().Rotation();
    FRotator NewRot = FMath::RInterpTo(Enemy->GetActorRotation(), TargetRot, DeltaSeconds, InterpSpeed);
    
    // 애니/컨트롤러 연동이 필요하면 Controller의 회전으로 바꿔줄 것
    Enemy->SetActorRotation(NewRot);

    // 도달 판단: Yaw 차이 절대값이 AcceptableYawError 이하이면 완료
    float YawDiff = FMath::Abs(FMath::UnwindDegrees(NewRot.Yaw - TargetRot.Yaw));
    if (YawDiff <= 2.0f)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
