// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "EPBTService_UpdateCombatState.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPBTService_UpdateCombatState : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
    UEPBTService_UpdateCombatState();

protected:
    /** 서비스가 활성화되어 있는 동안 주기적으로 호출되는 함수입니다. */
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    /** 이 서비스가 사용할 블랙보드 키들을 에디터에서 선택할 수 있도록 합니다. */
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetPlayerKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector CurrentStateKey;
};
