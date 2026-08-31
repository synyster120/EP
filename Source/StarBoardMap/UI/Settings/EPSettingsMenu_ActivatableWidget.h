#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonButtonBase.h"
#include "Settings/EPSettingTypes.h"
#include "EPSettingsMenu_ActivatableWidget.generated.h"

class UEPSliderRowWidget;

/**
 *  설정값 변경하는 위젯
 */

UCLASS()
class STARBOARDMAP_API UEPSettingsMenu_ActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UEPSliderRowWidget> MasterSliderRow;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UEPSliderRowWidget> BGMSliderRow;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UEPSliderRowWidget> SFXSliderRow;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UEPSliderRowWidget> VoiceSliderRow;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UEPSliderRowWidget> MousesensitivitySliderRow;

    // Setting Data
    FEPAudioSettings CurrentAudioSettings;
    FEPGameplaySettings CurrentGameplaySetting;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    TObjectPtr<UCommonButtonBase> Btn_Save;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonButtonBase> Btn_Close;

    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    // Common UI에서 최초 포커스 대상을 지정하는 가상 함수 오버라이드
    virtual UWidget* GetDesiredFocusTarget() const;

private:
    // Slider Binding function (파라미터는 딱 값 하나만 받음)
    UFUNCTION()
    void OnMasterVolumeChanged(float NewValue);

    UFUNCTION()
    void OnBGMVolumeChanged(float NewValue);

    UFUNCTION()
    void OnSFXVolumeChanged(float NewValue);

    UFUNCTION()
    void OnVoiceVolumeChanged(float NewValue);

    UFUNCTION()
    void OnMouseSensitivityChanged(float NewValue);



    UFUNCTION()
    void OnSaveButtonClicked();
    UFUNCTION()
    void OnCloseButtonClicked();


};
