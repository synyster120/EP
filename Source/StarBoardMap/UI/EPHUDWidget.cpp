// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EPHUDWidget.h"
#include "Components/Image.h"

void UEPHUDWidget::UpdateHealthFloat_Implementation(float NewHealth, float MaxHealth)
{
    // 이미지 포인터들이 유효한지 먼저 확인합니다.
    if (!Image1 || !Image2 || !Image3)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("HUD -- UpdateHealthfloat = %f, %f"), NewHealth, MaxHealth);
    // 최대 체력이 3일 때만 이 로직을 수행합니다.
    if (MaxHealth == 3.0f)
    {
        // NewHealth를 정수로 변환하여 비교합니다.
        const int32 CurrentHealth = FMath::FloorToInt(NewHealth);

        if (CurrentHealth == 2)
        {
            Image1->SetVisibility(ESlateVisibility::HitTestInvisible);
            Image2->SetVisibility(ESlateVisibility::HitTestInvisible);
            Image3->SetVisibility(ESlateVisibility::Hidden); // 3번만 숨김
        }
        else if (CurrentHealth == 1)
        {
            Image1->SetVisibility(ESlateVisibility::HitTestInvisible);
            Image2->SetVisibility(ESlateVisibility::Hidden); // 2번 숨김
            Image3->SetVisibility(ESlateVisibility::Hidden); // 3번 숨김
        }
        else if (CurrentHealth <= 0)
        {
            Image1->SetVisibility(ESlateVisibility::Hidden); // 전부 숨김
            Image2->SetVisibility(ESlateVisibility::Hidden);
            Image3->SetVisibility(ESlateVisibility::Hidden);
        }
        else // CurrentHealth가 3이거나 다른 값일 경우
        {
            // 모든 하트를 보이게 합니다.
            Image1->SetVisibility(ESlateVisibility::HitTestInvisible);
            Image2->SetVisibility(ESlateVisibility::HitTestInvisible);
            Image3->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
    }
    else
    {
        // 최대 체력이 3이 아닐 경우, 기본적으로 모든 하트를 보이게 처리합니다.
        Image1->SetVisibility(ESlateVisibility::HitTestInvisible);
        Image2->SetVisibility(ESlateVisibility::HitTestInvisible);
        Image3->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}
