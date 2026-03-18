// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/EPHealthBarStatComponent.h"
#include "Components/ProgressBar.h"
#include "Characters/EPPlayerCharacter.h"
#include "Components/Image.h"
#include "Chess/CKing.h"

void UChessUserWidget::NativeConstruct()
{
    Super::NativeConstruct();
	/*Image[0] = Image1;
	Image[1] = Image2;
	Image[2] = Image3;*/

    FString TargetActorName = TEXT("BP_CKing_C_0");

    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACKing::StaticClass(), AllActors);

    Player = Cast<AEPPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    King = nullptr;
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == TargetActorName)
        {
            King = Cast<ACKing>(Actor);
            break;
        }
    }
    if (!IsValid(King))
    {
        UE_LOG(LogTemp, Error, TEXT("ChessUserWidget: King not found (name containing 'BP_CKing')"));
        return;
    }
    if (!IsValid(King->StatComponent))
    {
        UE_LOG(LogTemp, Error, TEXT("ChessUserWidget: King->StatComponent is null"));
        return;
    }
    if (!IsValid(ProgressBar))
    {
        UE_LOG(LogTemp, Error, TEXT("ChessUserWidget: ProgressBar is null"));
        return;
    }

    King->StatComponent->OnHealthChanged.AddDynamic(this, &UChessUserWidget::OnHealthChange);

    //Player->StatComponent->OnHealthChanged.AddDynamic(this, &UChessUserWidget::OnPlayerHealthChange);
}

void UChessUserWidget::OnPlayerHealthChange()
{
    /*FEPHealthInfo INFO = Player->StatComponent->GetHealthInfo();
    for (int i = 0;i < 3;i++) {
        if (INFO.CurrentBlocks > i) Image[i]->SetVisibility(ESlateVisibility::Visible);
        else Image[i]->SetVisibility(ESlateVisibility::Hidden);
    }*/
}

void UChessUserWidget::OnHealthChange()
{
    FEPHealthInfo INFO = King->StatComponent->GetHealthInfo();
    ProgressBar->SetPercent(INFO.CurrentHealth / INFO.MaxHealth);
    UE_LOG(LogTemp, Warning, TEXT("HEALTH %f"), (INFO.MaxHealth - INFO.CurrentHealth) / INFO.MaxHealth);
}
