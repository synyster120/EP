// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EPEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "Perception/AISenseConfig_Sight.h"
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
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

    // 시야 감각(Sight Sense) 설정
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    


}

// 빙의되었을 때, 블랙보드 초기화 및 비헤이비어 실행
void AEPEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AEPEnemyCharacter* EnemyCharacter = Cast<AEPEnemyCharacter>(InPawn);
    if (EnemyCharacter && BehaviorTreeAsset)
    {

        if (SightConfig && EnemyCharacter)
        {
            //UE_LOG(LogTemp, Warning, TEXT("sightconfig or enemycharacter is not null | Radius 1 : %f , Radius 2 : %f "), EnemyCharacter->GetPerceptionRadius(), EnemyCharacter->GetLosePerceptionRadius());
            /*SightConfig->SightRadius = EnemyCharacter->GetPerceptionRadius();
            SightConfig->LoseSightRadius = EnemyCharacter->GetLosePerceptionRadius();*/
            SightConfig->SightRadius = 2000;
            SightConfig->LoseSightRadius = 1800;
            SightConfig->PeripheralVisionAngleDegrees = 90.0f;
            SightConfig->SetMaxAge(5.0f); // 인식을 잃은 후 5초간 기억
            SightConfig->DetectionByAffiliation.bDetectEnemies = true;
            SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
            SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

            // 한 프레임 뒤에 실행
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
                {
                    if (!PerceptionComponent->IsRegistered())
                    {
                        PerceptionComponent->RegisterComponent();
                    }
                    UE_LOG(LogTemp, Warning, TEXT("timer end"));
                    if (PerceptionComponent && SightConfig)
                    {
                        PerceptionComponent->ConfigureSense(*SightConfig);
                        PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
                        PerceptionComponent->RequestStimuliListenerUpdate();

                        SetPerceptionComponent(*PerceptionComponent);
                        GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEPEnemyAIController::AIPerceptionUpdated);
                    }
                }, 0.0f, false);

        }

        // 블랙보드 컴포넌트를 가져와서 지정된 블랙보드 애셋으로 초기화
        UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
        if (UseBlackboard(BehaviorTreeAsset->GetBlackboardAsset(), BlackboardComp))
        {
            // 캐릭터의 스탯으로 블랙보드 값들 초기화
            // (캐릭터에 GetStat() 함수가 구현되어 있다고 가정)
            UEPStatComponent* StatComponent = EnemyCharacter->GetStatComponent(); 
            StatComponent->OnStatInitialized.AddDynamic(this, &AEPEnemyAIController::OnStatReady);

        }
    }
}

void AEPEnemyAIController::BeginPlay()
{
    Super::BeginPlay();


    // 폰에 빙의했는지 확인 (안전장치)
    if (GetPawn())
    {
        // 폰으로부터 스탯 컴포넌트를 직접 가져와서 Perception을 설정합니다.
        // OnPossess가 BeginPlay보다 먼저 호출되므로, 이 시점에는 스탯이 이미 초기화되어 있습니다.
        AEPEnemyCharacter* EnemyCharacter = Cast<AEPEnemyCharacter>(GetPawn());

        //if (SightConfig && EnemyCharacter)
        //{
        //    UE_LOG(LogTemp, Warning, TEXT("sightconfig or enemycharacter is not null | Radius 1 : %f , Radius 2 : %f "), EnemyCharacter->GetPerceptionRadius(), EnemyCharacter->GetLosePerceptionRadius());
        //    SightConfig->SightRadius = EnemyCharacter->GetPerceptionRadius();
        //    SightConfig->LoseSightRadius = EnemyCharacter->GetLosePerceptionRadius();
        //    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
        //    SightConfig->SetMaxAge(5.0f); // 인식을 잃은 후 5초간 기억
        //    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        //    SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
        //    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

        //    // 한 프레임 뒤에 실행
        //    FTimerHandle TimerHandle;
        //    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
        //        {
        //            if (!PerceptionComponent->IsRegistered())
        //            {
        //                PerceptionComponent->RegisterComponent();
        //            }
        //            UE_LOG(LogTemp, Warning, TEXT("timer end"));
        //            if (PerceptionComponent && SightConfig)
        //            {
        //                PerceptionComponent->ConfigureSense(*SightConfig);
        //                PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
        //                PerceptionComponent->RequestStimuliListenerUpdate();

        //                SetPerceptionComponent(*PerceptionComponent);
        //                GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEPEnemyAIController::AIPerceptionUpdated);
        //            }
        //        }, 0.0f, false);



            // 설정된 시야 감각을 Perception Component에 추가
            /*AIPerceptionComponent->ConfigureSense(*SightConfig);
            AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

            AIPerceptionComponent->RequestStimuliListenerUpdate();
            SetPerceptionComponent(*AIPerceptionComponent);

            GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEPEnemyAIController::AIPerceptionUpdated);*/

        //}
        //else
        //{
        //    UE_LOG(LogTemp, Warning, TEXT("sightconfig or enemycharacter is null"));
        //}
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ai controller -->  not set is pawn"));
    }
}

void AEPEnemyAIController::OnStatReady(const FEPBaseStat& CurrentBaseStat)
{
    const FEPEnemyStat* EnemyStatPtr = static_cast<const FEPEnemyStat*>(&CurrentBaseStat);

    UE_LOG(LogTemp, Warning, TEXT("OnStatReady"));

    if (EnemyStatPtr)
    {
        InitializeBlackboard(*EnemyStatPtr);
        // 비헤이비어 트리 실행
        RunBehaviorTree(BehaviorTreeAsset);
    }

    //if (SightConfig)
    //{
    //    SightConfig->SightRadius = EnemyStatPtr->PerceptionRadius;
    //    SightConfig->LoseSightRadius = EnemyStatPtr->LosePerceptionRadius;
    //    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    //    SightConfig->SetMaxAge(5.0f); // 인식을 잃은 후 5초간 기억
    //    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    //    SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
    //    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    //    // 설정된 시야 감각을 Perception Component에 추가
    //    AIPerceptionComponent->ConfigureSense(*SightConfig);
    //    AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    //}
    // Perception Component의 델리게이트에 함수 바인딩
    //AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEPEnemyAIController::OnPerceptionUpdated);
}

// 블랙보드 초기화
void AEPEnemyAIController::InitializeBlackboard(const FEPEnemyStat& EnemyStat)
{
    UE_LOG(LogTemp, Warning, TEXT("ai - on InitializeBlackboard  [ this : %s]"), *GetName());

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
void AEPEnemyAIController::AIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    UE_LOG(LogTemp, Warning, TEXT("ai - on perception  [ target : %s]"), *Actor->GetName());

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
