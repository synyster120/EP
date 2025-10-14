// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EPBTTask_PlayMontageFromBB.generated.h"

/**
 *		BlackBoard 에 있는 Montage Play 하는 Task
 */
UCLASS()
class STARBOARDMAP_API UEPBTTask_PlayMontageFromBB : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MontageToPlayKey;

private:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> MyOwnerComp;

	static const FName IsHitKey;

protected:
	UEPBTTask_PlayMontageFromBB();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 비헤이비어 트리에 의해 이 태스크가 중단될 때 호출되는 함수
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 몽타주 재생이 끝났을 때 호출될 콜백 함수
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
