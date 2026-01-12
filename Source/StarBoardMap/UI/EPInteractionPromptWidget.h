// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "EPInteractionPromptWidget.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPInteractionPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // 블루프린트에서 호출할 수 있는 함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetItemName(FText NewName);

protected:
    // BindWidget 메타 태그: 블루프린트에 있는 같은 이름의 위젯을 자동으로 C++ 변수와 연결해줌 (매우 중요!)
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Txt_Name;
};
