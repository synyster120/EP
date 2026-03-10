// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "EPLoadingSubsystem.generated.h"

class UEPLoadingWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingProgress, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingFinished);

/**
 * 
 */

// Loading State
UENUM(BlueprintType)
enum class EEPLoadingState : uint8
{
    LoadingAssets           UMETA(DisplayName = "에셋 로딩"), 
    LoadingFX               UMETA(DisplayName = "FX 로딩"),
    LoadingStreamLevel      UMETA(DisplayName = "Level 로딩")
};

UCLASS()
class STARBOARDMAP_API UEPLoadingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    // ============================ Loading ============================
    // Subsystem 생명주기
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // 외부에서 호출할 로딩 요청 함수
    UFUNCTION(BlueprintCallable)
    void LoadLevelWithAssets(FName LevelName, TSoftObjectPtr<UPrimaryDataAsset> AssetLabel, FName LevelEntranceTag);

    UPROPERTY(BlueprintAssignable)
    FOnLoadingProgress OnLoadingProgress;

    UPROPERTY(BlueprintAssignable)
    FOnLoadingFinished OnLoadingFinished;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pawn")
    TSubclassOf<APawn> PlayerPawnClass;


    // ============================ UI ============================
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading")
    TSubclassOf<UEPLoadingWidget> LoadingWidgetClass;

    UFUNCTION(BlueprintCallable)
    void ShowLoadingScreen();

    UFUNCTION(BlueprintCallable)
    void HideLoadingScreen();

private:
    // ============================ Loading ============================
    void CheckProgress();
    UFUNCTION()
    void ProcessLoading();
    UFUNCTION()
    void OnAssetsDone();
    UFUNCTION()
    void OnFXLoadDone();

    UFUNCTION()
    void OnAllFinished();
    void OpenTargetLevel(FName LevelName);
    UFUNCTION()
    void OnLevelDataLoaded();
    void CallOpenTargetLevel();
    AActor* FindPlayerStartByTag(FName TagName); // 월드 내 PlayerStart 반환 함수

    void SetupPlayerPosition();

    TSharedPtr<FStreamableHandle> AssetLoadHandle;
    FTimerHandle TimerHandle_Progress;
    FName PendingLevelName;
    FName TargetPLevelEntranceTag;

    EEPLoadingState CurrentState = EEPLoadingState::LoadingAssets;
    FPrimaryAssetId CurrentLoadedAssetId;
    // ============================ UI ============================
    // 생성된 위젯을 들고 있을 포인터
    UPROPERTY()
    UEPLoadingWidget* CurrentWidget = nullptr;

    UFUNCTION()
    void HandleFadeOutFinished();
    UFUNCTION()
    void HandleFadeInFinished();
};
