#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Settings/EPSettingTypes.h"
#include "EPSaveSettings.generated.h"

/**
 * 
 */

UCLASS()
class STARBOARDMAP_API UEPSaveSettings : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FEPAudioSettings Audio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FEPGameplaySettings Gameplay;

};
