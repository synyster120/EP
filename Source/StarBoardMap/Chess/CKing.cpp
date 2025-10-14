// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CKing.h"
#include "Components/EPStatComponent.h"
#include "Components/EPHealthBarStatComponent.h"
#include "Data/EPCharacterTypes.h"

ACKing::ACKing()
{
}

void ACKing::BeginPlay()
{
    Name = FName("King");
    UnitType = 1;
    MovingTurn = true;
    Super::BeginPlay();
    UDataTable* MyTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/AssetDynamic/Data/Stats/DT_CharaterTypes_EnemyState.DT_CharaterTypes_EnemyState"));
    StatComponent->Initialize(*MyTable->FindRow<FEPBaseStat>(FName("CKing"), TEXT("EnemyStatContext")));
}

void ACKing::Attack()
{
}