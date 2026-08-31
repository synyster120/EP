
#include "UI/Settings/EPSliderRowWidget.h"
#include "Components/TextBlock.h"
#include "AnalogSlider.h"

void UEPSliderRowWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // 내부 슬라이더 이벤트에 바인딩
    if (MainSlider)
    {
        MainSlider->OnValueChanged.AddUniqueDynamic(this, &UEPSliderRowWidget::HandleInternalSliderValueChanged);
    }
}

void UEPSliderRowWidget::HandleInternalSliderValueChanged(float InValue)
{
    //내부 슬라이더 값이 변하면, 구독자(SettingsMenu)에게 방송
    OnValueChanged.Broadcast(InValue);
}

void UEPSliderRowWidget::SetTitleText(const FText& InText)
{
    if (Tx_SettingName)
    {
        Tx_SettingName->SetText(InText);
    }
}

void UEPSliderRowWidget::SetSliderValue(float NewValue)
{
    if (MainSlider)
    {
        MainSlider->SetValue(NewValue);
    }
}
