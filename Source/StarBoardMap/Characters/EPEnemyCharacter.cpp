// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EPEnemyCharacter.h"
#include "Components/EPHealthBarStatComponent.h"

AEPEnemyCharacter::AEPEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = false;


    // 구체적인 자식 컴포넌트를 생성
    UEPHealthBarStatComponent* EnemyStatComponent = CreateDefaultSubobject<UEPHealthBarStatComponent>(TEXT("StatComponent"));

    // 부모의 순수 C++ 포인터에 할당
    StatComponent = EnemyStatComponent;
}

