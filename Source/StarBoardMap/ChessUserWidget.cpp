// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/EPHealthBarStatComponent.h"
#include "Components/ProgressBar.h"
#include "Chess/CKing.h"

void UChessUserWidget::NativeConstruct()
{
	Image[0] = Image1;
	Image[1] = Image2;
	Image[2] = Image3;

    FString TargetActorName = TEXT("BP_CKing_C_0");

    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

    // 이름 비교로 액터 찾기
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == TargetActorName)
        {
            King = Cast<ACKing>(Actor);
            break;
        }
    }
    if (IsValid(King->StatComponent)) {
        King->StatComponent->OnHealthChanged.AddDynamic(this, &UChessUserWidget::OnHealthChange);
    }
}

void UChessUserWidget::OnHealthChange()
{
    FEPHealthInfo INFO = King->StatComponent->GetHealthInfo();
    ProgressBar->SetPercent(INFO.CurrentHealth / INFO.MaxHealth);
    UE_LOG(LogTemp, Warning, TEXT("HEALTH %f"), (INFO.MaxHealth - INFO.CurrentHealth) / INFO.MaxHealth);
}
