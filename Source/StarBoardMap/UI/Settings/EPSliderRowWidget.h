#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EPSliderRowWidget.generated.h"

/**
 *  하나의 슬라이더
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSliderValueChanged, float, NewValue);

UCLASS()
class STARBOARDMAP_API UEPSliderRowWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 슬라이더 값이 변할 때 방송할 델리게이트
    UPROPERTY(BlueprintAssignable)
    FOnSliderValueChanged OnValueChanged;

    // 내부의 슬라이더
    UPROPERTY(meta = (BindWidget))
    class UAnalogSlider* MainSlider;

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetTitleText(const FText& InText); // FText는 무겁기 때문에 const 참조자(&)로 넘김(정석)

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetSliderValue(float NewValue);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Tx_SettingName; // CommonTextBlock도 가능
    
    // 한번만 실행
    virtual void NativeOnInitialized() override;

private:
    // 내부 슬라이더의 변경 이벤트를 받을 콜백 함수
    UFUNCTION()
    void HandleInternalSliderValueChanged(float InValue);

};
