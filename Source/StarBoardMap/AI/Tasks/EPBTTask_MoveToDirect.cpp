// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/EPBTTask_MoveToDirect.h"
#include "EPBTTask_MoveToDirect.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

UEPBTTask_MoveToDirect::UEPBTTask_MoveToDirect()
{
    NodeName = TEXT("MoveToDirect (Debug)");
    bNotifyTick = false;
    // Object 타입(Actor)만 선택하도록
    TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UEPBTTask_MoveToDirect, TargetKey), AActor::StaticClass());
}

EBTNodeResult::Type UEPBTTask_MoveToDirect::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UE_LOG(LogTemp, Warning, TEXT("MoveTo ExecuteTask started"));
    AAIController* AI = OwnerComp.GetAIOwner();
    if (!AI) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    AActor* Goal = Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));
    if (!Goal) {
        UE_LOG(LogTemp, Warning, TEXT("MoveToDirect: Goal is null"));
        return EBTNodeResult::Failed;
    }

    FAIMoveRequest Req;
    Req.SetGoalActor(Goal);
    Req.SetProjectGoalLocation(true);
    Req.SetAllowPartialPath(true);
    Req.SetAcceptanceRadius(100.f);
    Req.SetUsePathfinding(true);

    FNavPathSharedPtr OutPath;
    auto Code = AI->MoveTo(Req, &OutPath);

    UE_LOG(LogTemp, Warning, TEXT("MoveToDirect: Request=%d HasPath=%d Pts=%d Goal=%s"),
        (int32)Code, OutPath.IsValid(), OutPath.IsValid() ? OutPath->GetPathPoints().Num() : 0,
        *GetNameSafe(Goal));

    // 대기형 태스크로 전환(완료는 메시지에서 받음)
    if (Code == EPathFollowingRequestResult::RequestSuccessful || Code == EPathFollowingRequestResult::AlreadyAtGoal) {
        // PathFollowing 완료 메시지를 수신하도록 설정
        WaitForMessage(OwnerComp, UBrainComponent::AIMessage_MoveFinished);
        return EBTNodeResult::InProgress;
    }
    return EBTNodeResult::Failed;
}

void UEPBTTask_MoveToDirect::OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess)
{
    UE_LOG(LogTemp, Warning, TEXT("MoveToDirect: Finished Message=%s Success=%d"), *Message.ToString(), bSuccess);
    FinishLatentTask(OwnerComp, bSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
}