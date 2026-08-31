
#include "Settings/EPSettingsSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/EPSaveSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "Settings/EPAudioDeveloperSettings.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

void UEPSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // sound mixer 활성화 (최상위 월드)
    //UGameplayStatics::PushSoundMixModifier(GetWorld(), MainSoundMix);

    LoadSettings(); // save data load 및 적용
}

// -------------------------------- disk save & load --------------------------------
void UEPSettingsSubsystem::LoadSettings()
{
    // 디스크에서 커스텀 설정(.sav) 로드
    if (UGameplayStatics::DoesSaveGameExist(SettingsSaveSlotName, 0))
    {
        CurrentSettings = Cast<UEPSaveSettings>(UGameplayStatics::LoadGameFromSlot(SettingsSaveSlotName, 0));
        UE_LOG(LogTemp, Warning, TEXT("load is save settings"));
    }
    else
    {
        // save가 없으면 새로 생성
        CurrentSettings = Cast<UEPSaveSettings>(UGameplayStatics::CreateSaveGameObject(UEPSaveSettings::StaticClass()));
    }

    // save setting 적용
    if (CurrentSettings)
    {
        ApplyAudioSettings(CurrentSettings->Audio);
        ApplyGameplaySettings(CurrentSettings->Gameplay);
    }
}

void UEPSettingsSubsystem::SaveAllSettings()
{
    if (!CurrentSettings) 
    {
        UE_LOG(LogTemp, Warning, TEXT("setting value is null - save fail"));
        return;
    }

    // save 설정(.sav) 디스크 저장
    UGameplayStatics::SaveGameToSlot(CurrentSettings, SettingsSaveSlotName, 0);

    // 그래픽 설정(.ini) 디스크 저장
    if (UGameUserSettings* UserSettings = GEngine->GetGameUserSettings())
    {
        UserSettings->ApplySettings(false); // 해상도, 창모드 적용 후
        UserSettings->SaveSettings();       // 디스크에 저장
    }

    UE_LOG(LogTemp, Warning, TEXT("save is success"));
}

// preview * 원본 세이브 데이터는 건드리지 않고, AudioMixer의 볼륨만 실시간으로 임시 변경 *
void UEPSettingsSubsystem::PreviewAudioVolume(EEPAudioChannel Channel, float Volume)
{
    // project settings에 넣어둔 값 가져와서 사용
    const UEPAudioDeveloperSettings* AudioSettings = GetDefault<UEPAudioDeveloperSettings>();

    if (!AudioSettings) return;

    // 목표 채널에 맞는 에셋 로드(동기)
    USoundClass* TargetClass = nullptr;

    switch (Channel)
    {
    case EEPAudioChannel::Master:
        TargetClass = AudioSettings->MasterSoundClass.LoadSynchronous();
        break;
    case EEPAudioChannel::BGM:
        TargetClass = AudioSettings->BGMSoundClass.LoadSynchronous();
        break;
    case EEPAudioChannel::SFX:
        TargetClass = AudioSettings->SFXSoundClass.LoadSynchronous();
        break;
    case EEPAudioChannel::Voice:
        TargetClass = AudioSettings->VoiceSoundClass.LoadSynchronous();
        break;
    }

    // 엔진의 오디오 믹서에 적용 (예시)
    if (TargetClass)
    {
        UGameplayStatics::SetSoundMixClassOverride(GetWorld(), AudioSettings->MainSoundMix.LoadSynchronous(), TargetClass, Volume, 1.0f, 0.0f);
    }
}

// -------------------------------- audio getter/setter --------------------------------
FEPAudioSettings UEPSettingsSubsystem::GetAudioSettings() const
{
    if (CurrentSettings)
    {
        return CurrentSettings->Audio; // 복사 후 반환
    }
    return FEPAudioSettings(); // 빈 구조체 반환
}

void UEPSettingsSubsystem::ApplyAudioSettings(const FEPAudioSettings& NewAudioSettings)
{
    if (!CurrentSettings)
    {
        UE_LOG(LogTemp, Warning, TEXT("setting value is null - apply audio fail"));
        return;
    }

    const UEPAudioDeveloperSettings* AudioSettings = GetDefault<UEPAudioDeveloperSettings>();
    if (AudioSettings && AudioSettings->MainSoundMix.LoadSynchronous())
    {
        // 사운드 믹스 push(활성화)
        UGameplayStatics::PushSoundMixModifier(GetWorld(), AudioSettings->MainSoundMix.Get());

        // 원본 데이터(메모리) 갱신
        CurrentSettings->Audio = NewAudioSettings;
        // 로드한 볼륨 값들을 오디오 엔진에 실제 세팅
        PreviewAudioVolume(EEPAudioChannel::Master, CurrentSettings->Audio.MasterVolume);
        PreviewAudioVolume(EEPAudioChannel::BGM, CurrentSettings->Audio.BGMVolume);
        PreviewAudioVolume(EEPAudioChannel::SFX, CurrentSettings->Audio.SFXVolume);
        PreviewAudioVolume(EEPAudioChannel::Voice, CurrentSettings->Audio.VoiceVolume);
    }
}

// -------------------------------- gameplay getter/setter --------------------------------
FEPGameplaySettings UEPSettingsSubsystem::GetGameplaySettings() const
{
    if (CurrentSettings)
    {
        return CurrentSettings->Gameplay; // 복사 후 반환
    }
    return FEPGameplaySettings(); // 빈 구조체 반환
}

void UEPSettingsSubsystem::ApplyGameplaySettings(const FEPGameplaySettings& NewGameplaySettings)
{
    if (!CurrentSettings)
    {
        UE_LOG(LogTemp, Warning, TEXT("setting value is null - apply gameplay fail"));
        return;
    }

    // 원본 데이터(메모리) 갱신
    CurrentSettings->Gameplay = NewGameplaySettings;


    // (해상도 관련 설정 즉시 적용하는 로직 추가) = preview

    // 마우스 감도
    // 실제 엔진에 즉시 적용 (예: PlayerController나 InputSettings 활용)
    // (UE 5.4 Enhanced Input의 Modifier 등을 조절하거나, PlayerController의 Yaw/Pitch Scale 변경)
    if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
    {
        // 간단한 예시
        if (APawn* PlayerPawn = PlayerController->GetPawn())
        {
            // Pawn의 기본 회전 속도 변수를 찾아 직접 수정
            // Cast<AEPCharacter>(PlayerPawn)->BaseLookUpRate = NewSensitivity * 기본값;

           // CurrentSettings->Gameplay.MouseSensitivity;
        }
    }
}
