#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EPAudioDeveloperSettings.generated.h"

class USoundMix;

/**
 *      사운드 관련 에셋 관리
 */

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "EP Audio Settings"))
class STARBOARDMAP_API UEPAudioDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
    // project settings 내 카테고리 이름 지정
    virtual FName GetCategoryName() const override { return FName("Eclipse Game Settings"); }

    // gameplay sound
    UPROPERTY(config, EditAnywhere, Category = "Audio Assets")
    TSoftObjectPtr <USoundMix> MainSoundMix;

    UPROPERTY(config, EditAnywhere, Category = "Audio Assets")
    TSoftObjectPtr<USoundClass> MasterSoundClass;

    UPROPERTY(config, EditAnywhere, Category = "Audio Assets")
    TSoftObjectPtr<USoundClass> BGMSoundClass;

    UPROPERTY(config, EditAnywhere, Category = "Audio Assets")
    TSoftObjectPtr<USoundClass> SFXSoundClass;

    UPROPERTY(config, EditAnywhere, Category = "Audio Assets")
    TSoftObjectPtr<USoundClass> VoiceSoundClass;


    // Loading
    UPROPERTY(config, EditAnywhere, Category = "Loading Audio Settings")
    TSoftObjectPtr <USoundMix> LoadingSoundMix;

    UPROPERTY(config, EditAnywhere, Category = "Loading Audio Settings")
    TSoftObjectPtr<USoundBase> Loading_BGMSound;
};
