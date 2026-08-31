#pragma once

#include "CoreMinimal.h"
#include "EPSettingTypes.generated.h"

/**
 *      setting 관련 구조체 정의
 */

// --- save 관련 구조체 ---
USTRUCT(BlueprintType)
struct FEPAudioSettings // 사운드
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) 
    float MasterVolume = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) 
    float BGMVolume = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) 
    float SFXVolume = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float VoiceVolume = 1.0f;
};

USTRUCT(BlueprintType)
struct FEPGameplaySettings // 게임 조작
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) 
    float MouseSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite) 
    bool bInvertYAxis = false;

};

// --- 그 외 ---
UENUM(BlueprintType)
enum class EEPAudioChannel : uint8
{
    Master,
    BGM,
    SFX,
    Voice
};

// 해상도