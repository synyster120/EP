// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CKing.h"
#include "Components/EPStatComponent.h"
#include "Components/EPHealthBarStatComponent.h"
#include "Data/EPCharacterTypes.h"
#include "Core/ChessGameMode.h"

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

void ACKing::OnDied()
{
    Super::OnDied();

    if (StatComponent)
    {
        StatComponent->OnHealthChanged.Broadcast();
    }

    AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        UE_LOG(LogTemp, Warning, TEXT("gamemode clear"));
        GameMode->ClearGame();
    }
}
