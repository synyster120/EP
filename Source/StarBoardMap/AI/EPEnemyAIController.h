// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EPEnemyAIController.generated.h"

struct FEPEnemyStat;
/**
 * 
 */
UCLASS()
class STARBOARDMAP_API AEPEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEPEnemyAIController();

    // -- 블랙보드 키 이름들 --
    // 코드에서 "TargetPlayer" 같은 문자열을 직접 쓰는 대신, 변수로 관리하여 실수를 방지합니다.
    static const FName TargetPlayerKey;
    static const FName SelfActorKey;
    static const FName CurrentStateKey;

    /** 빙의된 Pawn의 스탯으로 블랙보드를 초기화합니다. */
    void InitializeBlackboard(const FEPEnemyStat& EnemyStat);

protected:
    virtual void OnPossess(APawn* InPawn) override;

    // -- 컴포넌트 --
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    TObjectPtr<class UAIPerceptionComponent> AIPerceptionComponent;

    // -- 데이터 --
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

private:
    /** AI Perception Component가 인식을 업데이트했을 때 호출될 함수입니다. */
    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

};
