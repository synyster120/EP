
#include "UI/Settings/EPSettingsMenu_ActivatableWidget.h"
#include "UI/Settings/EPSliderRowWidget.h"
#include "Settings/EPSettingsSubsystem.h"

void UEPSettingsMenu_ActivatableWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // slider binding
    if (MasterSliderRow)
        MasterSliderRow->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnMasterVolumeChanged);

    if (BGMSliderRow)
        BGMSliderRow->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnBGMVolumeChanged);

    if (SFXSliderRow)
        SFXSliderRow->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnSFXVolumeChanged);

    if (VoiceSliderRow)
        VoiceSliderRow->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnVoiceVolumeChanged);

    if (MousesensitivitySliderRow)
        MousesensitivitySliderRow->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnMouseSensitivityChanged);

    // button binding
    if (Btn_Save)
    {
        Btn_Save->OnClicked().RemoveAll(this); // 초기화
        Btn_Save->OnClicked().AddUObject(this, &UEPSettingsMenu_ActivatableWidget::OnSaveButtonClicked);
    }
    if (Btn_Close)
    {
        Btn_Close->OnClicked().RemoveAll(this); // 초기화
        Btn_Close->OnClicked().AddUObject(this, &UEPSettingsMenu_ActivatableWidget::OnCloseButtonClicked);
    }
}

void UEPSettingsMenu_ActivatableWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("nativeconstruct and data setup"));
    // data setup
    if (UEPSettingsSubsystem* SettingsSystem = GetGameInstance()->GetSubsystem<UEPSettingsSubsystem>())
    {
        // load eetting 복사
        CurrentAudioSettings = GetGameInstance()->GetSubsystem<UEPSettingsSubsystem>()->GetAudioSettings();
        CurrentGameplaySetting = GetGameInstance()->GetSubsystem<UEPSettingsSubsystem>()->GetGameplaySettings();

        // audio settings
        if (MasterSliderRow)
            MasterSliderRow->SetSliderValue(CurrentAudioSettings.MasterVolume);
        if (BGMSliderRow)
            BGMSliderRow->SetSliderValue(CurrentAudioSettings.BGMVolume);
        if (SFXSliderRow)
            SFXSliderRow->SetSliderValue(CurrentAudioSettings.SFXVolume);
        if (VoiceSliderRow)
            VoiceSliderRow->SetSliderValue(CurrentAudioSettings.VoiceVolume);

        // gameplay settings
        if (MousesensitivitySliderRow)
            MousesensitivitySliderRow->SetSliderValue(CurrentGameplaySetting.MouseSensitivity);
    }
}

UWidget* UEPSettingsMenu_ActivatableWidget::GetDesiredFocusTarget() const
{
    // 창이 켜질 때 제일 먼저 포커스를 잡을 위젯 반환
    if (Btn_Close)
    {
        return Btn_Close;
    }

    return Super::GetDesiredFocusTarget();
}
// Preview binding
void UEPSettingsMenu_ActivatableWidget::OnMasterVolumeChanged(float NewValue)
{
    GetGameInstance()->GetSubsystem<UEPSettingsSubsystem>()->PreviewAudioVolume(EEPAudioChannel::Master, NewValue);
    CurrentAudioSettings.MasterVolume = NewValue;
}

void UEPSettingsMenu_ActivatableWidget::OnBGMVolumeChanged(float NewValue)
{
    GetGameInstance()->GetSubsystem<UEPSettingsSubsystem>()->PreviewAudioVolume(EEPAudioChannel::BGM, NewValue);
    CurrentAudioSettings.BGMVolume = NewValue;
}

void UEPSettingsMenu_ActivatableWidget::OnSFXVolumeChanged(float NewValue)
{
    GetGameInstance()->GetSubsystem<UEPSettingsSubsystem>()->PreviewAudioVolume(EEPAudioChannel::SFX, NewValue);
    CurrentAudioSettings.SFXVolume = NewValue;
}

void UEPSettingsMenu_ActivatableWidget::OnVoiceVolumeChanged(float NewValue)
{
    GetGameInstance()->GetSubsystem<UEPSettingsSubsystem>()->PreviewAudioVolume(EEPAudioChannel::Voice, NewValue);
    CurrentAudioSettings.VoiceVolume = NewValue;
}

void UEPSettingsMenu_ActivatableWidget::OnMouseSensitivityChanged(float NewValue)
{
    CurrentGameplaySetting.MouseSensitivity = NewValue;
}

// Button
void UEPSettingsMenu_ActivatableWidget::OnSaveButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Click save button"));
    if (UEPSettingsSubsystem* settingSubsystem = GetGameInstance()->GetSubsystem<UEPSettingsSubsystem>())
    {
        // save
        settingSubsystem->ApplyAudioSettings(CurrentAudioSettings);
        settingSubsystem->ApplyGameplaySettings(CurrentGameplaySetting);
        settingSubsystem->SaveAllSettings(); // 최종 save

        DeactivateWidget(); // 창 닫기
    }
}

void UEPSettingsMenu_ActivatableWidget::OnCloseButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Click cloase button"));
    if (UEPSettingsSubsystem* settingSubsystem = GetGameInstance()->GetSubsystem<UEPSettingsSubsystem>())
    {
        // reset & not save
        CurrentAudioSettings = settingSubsystem->GetAudioSettings();
        CurrentGameplaySetting = settingSubsystem->GetGameplaySettings();
        settingSubsystem->ApplyAudioSettings(CurrentAudioSettings);
        settingSubsystem->ApplyGameplaySettings(CurrentGameplaySetting);

        DeactivateWidget(); // 창 닫기
    }
}
