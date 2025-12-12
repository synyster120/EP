// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "EPEnemyAIController.generated.h"

struct FEPEnemyStat;
struct FEPBaseStat;
class AEPEnemyCharacter;
/**
 * 
 */
UCLASS()
class STARBOARDMAP_API AEPEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
    // -- 데이터 --
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAISenseConfig_Sight* SightConfig;

    float CurrentPerceptionRadius;

    float CurrentLosePerceptionRadius;

private:
    // -- 블랙보드 키 이름들 --
    static const FName TargetKey;
    static const FName SelfActorKey;
    static const FName CurrentStateKey;
    static const FName IsDeadKey;
    static const FName IsHitKey;
    static const FName MontageToPlayKey;
    static const FName IsWindupKey;
    static const FName HealthKey;


public:
	AEPEnemyAIController();

    // Blackboard 에 Die 상태 업데이트
    void NotifyDeathUpdate();

    // Blackboard 에 Hit 상태 업데이트
    void NotifyHitUpdate();

    // Blackboard 에 health 상태 업데이트
    void NotifyHealthUpdate(float CurrentHealth);

    // Blackboard 에 Play 할 Montage 업데이트
    void PlayMontageUpdate(UAnimMontage* CurrentMontage);

    void NotifyIsWindupUpdate(); // 블랙보드에 IsWindup 값 true 로 업데이트

    virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void BeginPlay() override;

    // Character 관련 데이터로 설정 및 초기화 함수 (바인딩)
    UFUNCTION()
    void OnCharacterReady();

    // Stat Data 기반 Perception 설정 update
    void UpdatePerception(AEPEnemyCharacter* MyEnemyCharacter);

    // 블랙보드 업데이트
    void InitializeBlackboardUpdate(AEPEnemyCharacter* MyEnemyCharacter);
    

private:
    //  Perception Component가 인식 업데이트 시 호출 함수
    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);


};
