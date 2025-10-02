// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/EPBTService_UpdateCombatState.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EPCombatCharacterBase.h" // 캐릭터의 정보를 가져오기 위해
#include "Components/EPStatComponent.h"       // 스탯 컴포넌트의 정보를 가져오기 위해
//#include "Core/Interfaces/EPCombatInterface.h" 
#include "Core/EPGameplayTags.h"


UEPBTService_UpdateCombatState::UEPBTService_UpdateCombatState()
{
    NodeName = TEXT("Update Combat State");
    Interval = 0.5f; // 0.5초마다 상황을 판단
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
    UObject* TargetObject = BlackboardComp->GetValueAsObject(TargetPlayerKey.SelectedKeyName);
    AActor* TargetPlayer = Cast<AActor>(TargetObject);

    FGameplayTag NewStateTag;

    if (!TargetPlayer)
    {
        // 타겟이 없으면 '순찰' 상태로 설정
        //NewStateTag = FGameplayTag::RequestGameplayTag(TEXT("AI.State.Patrol"));
        NewStateTag = FEPGameplayTags::Get().AI_State_Patrol;
    }
    else
    {
        // 타겟이 있으면, 거리와 상태를 기반으로 '전투' 상태를 결정
        const float DistanceToTarget = FVector::Dist(ControlledCharacter->GetActorLocation(), TargetPlayer->GetActorLocation());
        const float AttackRange = ControlledCharacter->GetStatComponent()->GetAttackRange(); // 스탯 컴포넌트에서 공격 사거리 조회

        if (DistanceToTarget <= AttackRange)
        {
            // 공격 범위 안 -> '공격' 상태
            //NewStateTag = FGameplayTag::RequestGameplayTag(TEXT("AI.State.Combat.Attack"));
            NewStateTag = FEPGameplayTags::Get().AI_State_Combat_Attack;
        }
        else
        {
            // 공격 범위 밖 -> '추격' 상태
            //NewStateTag = FGameplayTag::RequestGameplayTag(TEXT("AI.State.Combat.Chase"));
            NewStateTag = FEPGameplayTags::Get().AI_State_Combat_Chase;
        }
    }

    // 결정된 새로운 상태를 블랙보드의 CurrentStateKey에 기록 
    //BlackboardComp->SetValueAsTag(CurrentStateKey.SelectedKeyName, NewStateTag);

}
