// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EPEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Data/EPCharacterTypes.h"
#include "Characters/EPEnemyCharacter.h"
#include "Components/EPStatComponent.h"

// 블랙보드 키 이름 초기화
const FName AEPEnemyAIController::TargetPlayerKey(TEXT("TargetPlayer"));
const FName AEPEnemyAIController::SelfActorKey(TEXT("SelfActor"));
const FName AEPEnemyAIController::CurrentStateKey(TEXT("CurrentState"));

AEPEnemyAIController::AEPEnemyAIController()
{
    // AI Perception Component 생성
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

    // 시야 감각(Sight Sense) 설정
    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    if (SightConfig)
    {
        SightConfig->SightRadius = 1500.0f;
        SightConfig->LoseSightRadius = 2000.0f;
        SightConfig->PeripheralVisionAngleDegrees = 90.0f;
        SightConfig->SetMaxAge(5.0f); // 인식을 잃은 후 5초간 기억
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

        // 설정된 시야 감각을 Perception Component에 추가
        AIPerceptionComponent->ConfigureSense(*SightConfig);
        AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    }

    // Perception Component의 델리게이트에 함수 바인딩
    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEPEnemyAIController::OnPerceptionUpdated);

}

// 빙의되었을 때, 블랙보드 초기화 및 비헤이비어 실행
void AEPEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AEPEnemyCharacter* EnemyCharacter = Cast<AEPEnemyCharacter>(InPawn);
    if (EnemyCharacter && BehaviorTreeAsset)
    {
        // 블랙보드 컴포넌트를 가져와서 지정된 블랙보드 애셋으로 초기화
        UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
        if (UseBlackboard(BehaviorTreeAsset->GetBlackboardAsset(), BlackboardComp))
        {
            // 캐릭터의 스탯으로 블랙보드 값들 초기화
            // (캐릭터에 GetStat() 함수가 구현되어 있다고 가정)
            UEPStatComponent* StatComponent = EnemyCharacter->GetStatComponent();
            /*if (FEPEnemyStat* CurrentStat = Cast<FEPEnemyStat>(StatComponent->GetStatData()))
            {
                InitializeBlackboard(*CurrentStat);
            }*/
            //InitializeBlackboard();

            // 비헤이비어 트리 실행
            RunBehaviorTree(BehaviorTreeAsset);
        }
    }
}

// 블랙보드 초기화
void AEPEnemyAIController::InitializeBlackboard(const FEPEnemyStat& EnemyStat)
{
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsObject(SelfActorKey, GetPawn());
        BlackboardComp->SetValueAsFloat(TEXT("AttackRange"), EnemyStat.AttackRange);
        BlackboardComp->SetValueAsFloat(TEXT("SightRange"), EnemyStat.PerceptionRadius);
        // ... 기타 필요한 초기값 설정 ...
    }
}

// Player 인식 여부 판단 및 update
void AEPEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
    if (BlackboardComp)
    {
        // 인식이 성공했다면 TargetPlayer 키에 인식된 액터를 설정
        if (Stimulus.WasSuccessfullySensed())
        {
            BlackboardComp->SetValueAsObject(TargetPlayerKey, Actor);
        }
        else // 인식을 잃었다면 TargetPlayer 키를 비움
        {
            BlackboardComp->ClearValue(TargetPlayerKey);
        }
    }
}
