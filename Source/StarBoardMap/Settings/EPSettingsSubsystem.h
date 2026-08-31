#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Settings/EPSettingTypes.h"
#include "Settings/EPSaveSettings.h"
#include "EPSettingsSubsystem.generated.h"

/**
 * 
 */

UCLASS()
class STARBOARDMAP_API UEPSettingsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // ------------ save 데이터 로드 및 저장 ------------
    void LoadSettings();
    void SaveAllSettings(); // 실제 save 수행

    void PreviewAudioVolume(EEPAudioChannel Channel, float Volume); // setting 값은 ui가 소유, 믹서 적용(preview)은 subsystem


    // ------------ audio getter/setter ------------
    // getter (audio 데이터 읽기 제공)
    UFUNCTION(BlueprintCallable, Category = "Audio")
    FEPAudioSettings GetAudioSettings() const;

    // setter (변경된 설정값을 CurrentSettings 객체에 적용, 실제 save X)
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void ApplyAudioSettings(const FEPAudioSettings& NewAudioSettings);
    

    // ------------ gameplay getter/setter ------------
    // getter (gameplay 데이터 읽기 제공)
    UFUNCTION(BlueprintCallable, Category = "GamePlay")
    FEPGameplaySettings GetGameplaySettings() const;

    // setter (변경된 설정값을 CurrentSettings 객체에 적용, 실제 save X)
    UFUNCTION(BlueprintCallable, Category = "GamePlay")
    void ApplyGameplaySettings(const FEPGameplaySettings& NewGameplaySettings);


private:
    // 디스크(슬롯에 저장되는 데이터) 입출력을 위한 세이브 객체 포인터 (GC 방지)
    UPROPERTY()
    TObjectPtr<UEPSaveSettings> CurrentSettings;

    FString SettingsSaveSlotName = TEXT("EclipseSettingsSlot");

    
    // 오디오 믹서 제어를 위한 에셋
    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<class USoundMix> MainSoundMix;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundClass> MasterSoundClass;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundClass> BGMSoundClass;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundClass> SFXSoundClass;
};
