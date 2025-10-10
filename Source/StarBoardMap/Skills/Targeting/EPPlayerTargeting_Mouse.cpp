// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Targeting/EPPlayerTargeting_Mouse.h"
#include "Data/EPSkillTypes.h"
#include "GameFramework/Character.h"

bool UEPPlayerTargeting_Mouse::FindTarget(ACharacter* Caster, const FEPSkillPhaseData& PhaseData, FEPSkillTargetData& OutTargetData)
{
    APlayerController* PlayerController = Cast<APlayerController>(Caster->GetController());
    if (!PlayerController) return false;

    // 스킬 단계 데이터에서 타겟 타입을 직접 사용합니다.
    const EEPTargetType TargetingType = PhaseData.TargetType;
    OutTargetData.TargetType = TargetingType;

    switch (TargetingType)
    {
    case EEPTargetType::Self:
        OutTargetData.TargetActor = Caster;
        break;

    case EEPTargetType::Actor:
    {
        FHitResult HitResult;
        PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
        OutTargetData.TargetActor = HitResult.GetActor();
        break;
    }
    case EEPTargetType::Direction:
        OutTargetData.TargetDirection = PlayerController->GetControlRotation().Vector();
        break;

    case EEPTargetType::Location:
    {
        FHitResult HitResult;
        PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
        OutTargetData.TargetLocation = HitResult.Location;
        break;
    }
    default:
        return false; // 유효하지 않은 타겟 타입
    }

    return true; // 타겟 탐색 성공
}
