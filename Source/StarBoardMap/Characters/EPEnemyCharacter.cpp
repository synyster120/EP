// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EPEnemyCharacter.h"
#include "Components/EPHealthBarStatComponent.h"
#include "Components/EPSkillComponent.h"
#include "AI/EPEnemyAIController.h"

AEPEnemyCharacter::AEPEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = false;


    // 구체적인 자식 컴포넌트를 생성
    UEPHealthBarStatComponent* EnemyStatComponent = CreateDefaultSubobject<UEPHealthBarStatComponent>(TEXT("StatComponent"));

    // 부모의 순수 C++ 포인터에 할당
    StatComponent = EnemyStatComponent;
}

void AEPEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (SkillComponent)
    {
        SkillComponent->ActivateSkill(0);
    }
}

void AEPEnemyCharacter::InitializeCharacterData()
{
    Super::InitializeCharacterData();

    if (StatDataRowHandle.DataTable && !StatDataRowHandle.RowName.IsNone())
    {
        // 3. 이번에는 자식 타입인 FEPEnemyStat으로 데이터를 가져옵니다.
        const FEPEnemyStat* EnemyStatData = StatDataRowHandle.GetRow<FEPEnemyStat>(TEXT(""));
        if (EnemyStatData)
        {
            // 4. 이 클래스 고유의 AI 스탯들을 초기화합니다.
            PerceptionRadius = EnemyStatData->PerceptionRadius;
            LosePerceptionRadius = EnemyStatData->LosePerceptionRadius;

            // 5. AIController의 블랙보드를 초기화하는 로직도 이곳에서 처리하는 것이 이상적입니다.
            AEPEnemyAIController* AIController = Cast<AEPEnemyAIController>(GetController());
            if (AIController)
            {
                // AIController에게 블랙보드를 초기화하라고 명령합니다.
                AIController->InitializeBlackboard(*EnemyStatData);
            }
        }
    }
}

