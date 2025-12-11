// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EPHealthUnitWidget.generated.h"

/**
 * 
 */
UCLASS()
class STARBOARDMAP_API UEPHealthUnitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 외부(HUD)에서 함수 호출 (체력칸 상태 설정)
	UFUNCTION(BlueprintCallable, Category = "Health")
	void UpdateState(bool bIsFull);

protected:
	// C++에서는 선언만 하고, 구현(_Implementation)은 BP에서 함
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnStateChanged(bool bIsFull);
};
