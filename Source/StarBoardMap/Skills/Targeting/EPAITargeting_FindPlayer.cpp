// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Targeting/EPAITargeting_FindPlayer.h"
#include "Data/EPSkillTypes.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

// 블랙보드 키 이름 초기화
const FName UEPAITargeting_FindPlayer::TargetPlayerKey(TEXT("TargetPlayer"));


bool UEPAITargeting_FindPlayer::FindTarget(ACharacter* Caster, const FEPSkillPhaseData& PhaseData, FEPSkillTargetData& OutTargetData)
{
    AAIController* AIController = Cast<AAIController>(Caster->GetController());
    if (!AIController) return false;

    UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
    if (!Blackboard) return false;

    const EEPTargetType TargetingType = PhaseData.TargetType;
    OutTargetData.TargetType = TargetingType;

    switch (TargetingType)
    {
    case EEPTargetType::Self:
        OutTargetData.TargetActor = Caster;
        break;

    case EEPTargetType::Actor:
        // 블랙보드에 저장된 'TargetPlayer'를 타겟 액터로 설정합니다.
        if (Cast<AActor>(Blackboard->GetValueAsObject(TargetPlayerKey)))
        {
            OutTargetData.TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetPlayerKey));
        }
        else
        {
            return false;
        }
        break;

    case EEPTargetType::Direction:
    {
        // 타겟(플레이어)을 향하는 방향을 공격 방향으로 설정합니다.
        AActor* TargetPlayer = Cast<AActor>(Blackboard->GetValueAsObject(TargetPlayerKey));
        if (TargetPlayer)
        {
            OutTargetData.TargetDirection = (TargetPlayer->GetActorLocation() - Caster->GetActorLocation()).GetSafeNormal();
        }
        else
        {
            return false;
        }
        break;
    }
    case EEPTargetType::Location:
    {
        // 타겟(플레이어)의 현재 위치를 공격 위치로 설정합니다.
        AActor* TargetPlayer = Cast<AActor>(Blackboard->GetValueAsObject(TargetPlayerKey));
        if (TargetPlayer)
        {
            OutTargetData.TargetLocation = TargetPlayer->GetActorLocation();
        }
        break;
    }
    default:
        return false;
    }

    return true;
}
