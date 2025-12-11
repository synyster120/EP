// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/PanelWidget.h" // 컨테이너 부모 클래스
#include "EPHUDWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "HUD")
    void UpdateHealthFloat(float NewHealth, float MaxHealth);

protected:
    // UI 에디터에서 배치할 컨테이너 (HorizontalBox나 WrapBox 추천)
    // BindWidget을 사용하여 블루프린트의 위젯과 자동 연결
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPanelWidget> HealthContainer;

    // 생성할 체력 칸 위젯 클래스 (BP에서 설정)
    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    TSubclassOf<UUserWidget> HealthUnitClass;
};
