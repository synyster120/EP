// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EPEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "Perception/AISenseConfig_Sight.h"
#include "Data/EPCharacterTypes.h"
#include "Characters/EPEnemyCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Components/EPStatComponent.h"
#include "GenericTeamAgentInterface.h"


// 블랙보드 키 이름 초기화
const FName AEPEnemyAIController::TargetKey(TEXT("Target")); 
const FName AEPEnemyAIController::SelfActorKey(TEXT("SelfActor"));
const FName AEPEnemyAIController::CurrentStateKey(TEXT("CurrentState"));
const FName AEPEnemyAIController::IsDeadKey(TEXT("IsDead"));
const FName AEPEnemyAIController::IsHitKey(TEXT("IsHit"));
const FName AEPEnemyAIController::MontageToPlayKey(TEXT("MontageToPlay"));
const FName AEPEnemyAIController::IsWindupKey(TEXT("IsWindup"));

AEPEnemyAIController::AEPEnemyAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));

    // 시야 감지 설정
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 0.0f;
    SightConfig->LoseSightRadius = 0.0f;
    SightConfig->PeripheralVisionAngleDegrees = 180.0f; // 시야각
    SightConfig->SetMaxAge(2.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true; // 적대 관계
    SightConfig->DetectionByAffiliation.bDetectNeutrals = false; // 중립 관계
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false; // 팀 관계

    GetPerceptionComponent()->ConfigureSense(*SightConfig);
    GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
    GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEPEnemyAIController::OnPerceptionUpdated);

}

// 빙의되었을 때, 블랙보드 초기화 및 비헤이비어 실행
void AEPEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (const IGenericTeamAgentInterface* TeamAgent = Cast<const IGenericTeamAgentInterface>(InPawn))
    {
        // 빙의한 캐릭터의 TeamID를 나의 TeamID로 설정
        SetGenericTeamId(TeamAgent->GetGenericTeamId());
    }

    AEPEnemyCharacter* MyCharacter = Cast<AEPEnemyCharacter>(InPawn);
    if (MyCharacter)
    {
        // 캐릭터의 데이터 준비가 끝나면 OnCharacterReady 함수 호출
        MyCharacter->OnDataInitialized.AddDynamic(this, &AEPEnemyAIController::OnCharacterReady);
    }
}

void AEPEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

}

// Character 관련 데이터로 설정 및 초기화 함수 (바인딩)
void AEPEnemyAIController::OnCharacterReady()
{
    UE_LOG(LogTemp, Warning, TEXT("OnCharacterReady"));

    AEPEnemyCharacter* MyCharacter = Cast<AEPEnemyCharacter>(GetPawn());

    if (MyCharacter)
    {
        // 지각 업데이트 및 블랙보드 초기화
        UpdatePerception(MyCharacter);
        InitializeBlackboardUpdate(MyCharacter);

        // 비헤이비어 트리 실행
        RunBehaviorTree(BehaviorTreeAsset);
    }
}

// Stat Data 기반 Perception 설정 update
void AEPEnemyAIController::UpdatePerception(AEPEnemyCharacter* MyEnemyCharacter)
{
    if (MyEnemyCharacter)
    {
        if (SightConfig)
        {
            SightConfig->DetectionByAffiliation.bDetectEnemies = true;
            SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
            SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

            UE_LOG(LogTemp, Warning, TEXT("aicontroller : %f  // %f"), MyEnemyCharacter->GetPerceptionRadius(), MyEnemyCharacter->GetLosePerceptionRadius());
            
            SightConfig->SightRadius = MyEnemyCharacter->GetPerceptionRadius();
            SightConfig->LoseSightRadius = MyEnemyCharacter->GetLosePerceptionRadius();

            GetPerceptionComponent()->ConfigureSense(*SightConfig);
            GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
            GetPerceptionComponent()->SetSenseEnabled(UAISense_Sight::StaticClass(), true);
            GetPerceptionComponent()->RequestStimuliListenerUpdate();

        }
    }
}

// 블랙보드 초기화
void AEPEnemyAIController::InitializeBlackboardUpdate(AEPEnemyCharacter* MyEnemyCharacter)
{
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();

    if (BlackboardComp && MyEnemyCharacter)
    {
        BlackboardComp->SetValueAsObject(SelfActorKey, GetPawn());
        BlackboardComp->SetValueAsFloat(TEXT("AttackRange"), MyEnemyCharacter->GetStatComponent()->GetAttackRange());
        BlackboardComp->SetValueAsFloat(TEXT("SightRange"), MyEnemyCharacter->GetPerceptionRadius());
        // ... 기타 필요한 초기값 설정 ...
    }
}

// Player 인식 여부 판단 및 update
void AEPEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
    if (!BlackboardComp) return;

    // 감지된 액터가 플레이어 캐릭터 확인
    if (Actor)
    {
        // 시전자의 팀 ID를 가져옴
        FGenericTeamId CasterTeamId = FGenericTeamId::NoTeam; // 기본값 = '팀 없음'
        if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn()))
        {
            CasterTeamId = TeamAgent->GetGenericTeamId();
        }

        // perception actor 가 같은 팀인지 확인
        if (CasterTeamId != FGenericTeamId::NoTeam) // 시전자가 팀이 있을 경우에만 검사
        {
            IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(Actor);
            // 대상이 같은 팀이라면, 함수 탈출
            if (TargetTeamAgent && TargetTeamAgent->GetGenericTeamId() == CasterTeamId)
            {
                UE_LOG(LogTemp, Warning, TEXT("Target actor is team --> Blackboard not update : %s"), *Actor->GetName());
                return;
            }
        }

        if (Stimulus.WasSuccessfullySensed())
        {
            // 인지 성공
            BlackboardComp->SetValueAsObject(TargetKey, Actor);
            UE_LOG(LogTemp, Warning, TEXT("Target actor -> Blackboard set update : %s"), *Actor->GetName());
        }
        else
        {
            if (!BlackboardComp->GetValueAsBool(IsWindupKey)) // IsWindup일 경우 타겟 놓치는 거 방지
            {
                // 인지 실패
                BlackboardComp->ClearValue(TargetKey);
                UE_LOG(LogTemp, Warning, TEXT("Target actor -> Blackboard clear update : %s"), *Actor->GetName());
            }
        }
    }
}

// Blackboard 에 Die 상태 업데이트
void AEPEnemyAIController::NotifyDeathUpdate()
{
    UE_LOG(LogTemp, Warning, TEXT("OnDied - notify death update -- ok"));
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();

    // 인지 컴포넌트 비활성화, target 리셋

    if (BlackboardComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnDied - notify death update -- Success"));
        BlackboardComp->SetValueAsBool(IsDeadKey, true);
    }
}

// Blackboard 에 Hit 상태 업데이트
void AEPEnemyAIController::NotifyHitUpdate()
{
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();

    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsBool(IsHitKey, true);
    }
}

// Blackboard 에 Play 할 Montage 업데이트
void AEPEnemyAIController::PlayMontageUpdate(UAnimMontage* CurrentMontage)
{
    if (!CurrentMontage) return;

    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();

    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsObject(MontageToPlayKey, CurrentMontage);
    }
}

void AEPEnemyAIController::NotifyIsWindupUpdate()
{
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
    
    if (BlackboardComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("IsWindup is false --> blackboard update true"));
        BlackboardComp->SetValueAsBool(IsWindupKey, true); // 대기 상태로 전환
    }
}

ETeamAttitude::Type AEPEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    // 빙의한 폰이 있는지 확인
    if (const APawn* MyPawn = GetPawn())
    {
        // 나와 상대방이 모두 팀 인터페이스를 가지고 있는지 확인
        if (const IGenericTeamAgentInterface* TeamAgent = Cast<const IGenericTeamAgentInterface>(&Other))
        {
            // 두 액터의 팀 ID를 비교합니다.
            FGenericTeamId OtherTeamId = TeamAgent->GetGenericTeamId();

            if (OtherTeamId == GetGenericTeamId())
            {
                // 팀 ID가 같으면 '아군(Friendly)'
                return ETeamAttitude::Friendly;
            }
            else
            {
                // 팀 ID가 다르면 '적(Hostile)'
                return ETeamAttitude::Hostile;
            }
        }
    }

    // 어떤 경우에도 해당하지 않으면 '중립(Neutral)'
    return ETeamAttitude::Neutral;
}

