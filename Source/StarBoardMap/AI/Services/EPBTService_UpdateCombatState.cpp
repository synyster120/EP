// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/EPBTService_UpdateCombatState.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EPCombatCharacterBase.h" // 캐릭터의 정보를 가져오기 위해
#include "Components/EPStatComponent.h"       // 스탯 컴포넌트의 정보를 가져오기 위해
//#include "Core/Interfaces/EPCombatInterface.h" 
//#include "Core/EPGameplayTags.h"
#include "Data/EPCharacterTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

// 블랙보드 키 이름 초기화
const FName UEPBTService_UpdateCombatState::TargetKey(TEXT("Target"));
const FName UEPBTService_UpdateCombatState::CurrentStateKey(TEXT("CurrentState"));

UEPBTService_UpdateCombatState::UEPBTService_UpdateCombatState()
{
    NodeName = TEXT("EP Update Combat State");
    bNotifyTick = true;
    bCallTickOnSearchStart = true;
    Interval = 1.5f; // 0.5초마다 상황 판단

    UE_LOG(LogTemp, Warning, TEXT("Service ok"));
    
}

void UEPBTService_UpdateCombatState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    AEPCombatCharacterBase* ControlledCharacter = Cast<AEPCombatCharacterBase>(AIController->GetPawn());

    if (!AIController || !BlackboardComp || !ControlledCharacter)
    {
        return;
    }

    // 블랙보드에서 타겟 플레이어 정보를 가져옴
    UObject* TargetObject = BlackboardComp->GetValueAsObject(TargetKey);
    AActor* TargetPlayer = Cast<AActor>(TargetObject);

    EEPAIState NewState;

    if (!TargetPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("TargetPlayer is null"));
        // 타겟이 없으면 '순찰' 상태로 설정
        NewState = EEPAIState::EAS_Patrolling;
        //NewStateTag = FEPGameplayTags::Get().AI_State_Patrol;
        //NewStateTag = FGameplayTag::RequestGameplayTag(TEXT("AI.State.Patrol"));
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("target : %s"), *TargetPlayer->GetName());
        // 
        //FAIMoveRequest Req;
        //Req.SetGoalActor(TargetPlayer);
        //Req.SetAcceptanceRadius(100.f);          // 널널히
        //Req.SetAllowPartialPath(true);
        //Req.SetUsePathfinding(true);
        //Req.SetProjectGoalLocation(true);

        //FNavPathSharedPtr OutPath;
        //auto Code = AIController->MoveTo(Req, &OutPath);
        //UE_LOG(LogTemp, Warning, TEXT("MoveTo Code=%d HasPath=%d NumPts=%d"),
        //    (int32)Code, OutPath.IsValid(), OutPath.IsValid() ? OutPath->GetPathPoints().Num() : 0);

        //AIController->GetPathFollowingComponent()->OnRequestFinished.AddLambda(
        //    [](FAIRequestID, const FPathFollowingResult& Res) {
        //        UE_LOG(LogTemp, Warning, TEXT("PF finished: %s"),
        //            *UEnum::GetValueAsString(Res.Code)); // Success, Blocked, Aborted, Invalid, AlreadyAtGoal
        //    });


        // 타겟이 있으면, 거리와 상태를 기반으로 '전투' 상태를 결정
        const float DistanceToTarget = FVector::Dist(ControlledCharacter->GetActorLocation(), TargetPlayer->GetActorLocation());
        const float AttackRange = ControlledCharacter->GetStatComponent()->GetAttackRange(); // 스탯 컴포넌트에서 공격 사거리 조회

        if (DistanceToTarget <= AttackRange)
        {
            // 공격 범위 안 -> '공격' 상태
            NewState = EEPAIState::EAS_Attacking;
        }
        else
        {
            // 공격 범위 밖 -> '추격' 상태
            NewState = EEPAIState::EAS_Chasing;
        }
    }

    if ((uint8)NewState != BlackboardComp->GetValueAsEnum(CurrentStateKey))
    {
        UE_LOG(LogTemp, Warning, TEXT("service -- current state - update"));
        // 결정된 새로운 상태를 블랙보드의 CurrentStateKey에 기록 
        BlackboardComp->SetValueAsEnum(CurrentStateKey, (uint8)NewState);
    }

}
