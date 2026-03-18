// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Subsystems/EPLoadingSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Data/EPFXPreloadLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/EPLoadingWidget.h"

#include "Engine/LevelStreaming.h"
#include "UObject/UObjectGlobals.h"

#include "NavigationSystem.h"

#include "Gimmick/EPLevelEntrance.h"
#include "Core/Subsystems/EPRespawnSubsystem.h"

#include "LevelElements/EPMonsterSpawner.h"

// ============================ Loading ============================
void UEPLoadingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // 초기화
    LoadingWidgetClass = APawn::StaticClass(); // 기본값
}

void UEPLoadingSubsystem::Deinitialize()
{
    // 위젯 제거
     if (CurrentWidget)
     {
         CurrentWidget->RemoveFromParent();
         CurrentWidget = nullptr;
     }

    // 뷰포트에 남아있을지 모르는 UI 포커스를 게임으로 강제 복구
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PlayerController = World->GetFirstPlayerController())
        {
            FInputModeGameOnly InputMode;
            PlayerController->SetInputMode(InputMode);
            PlayerController->bShowMouseCursor = false;
        }
    }

    Super::Deinitialize();
}

// 외부에서 호출할 로딩 요청 함수
void UEPLoadingSubsystem::LoadLevelWithAssets(FName LevelName, TSoftObjectPtr<UPrimaryDataAsset> AssetLabel, FName LevelEntranceTag)
{
    ShowLoadingScreen();

    // 월드 사운드 제거
    MuteWorldAudio();

    // 플레이어 컨트롤러 입력 잠금
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->SetIgnoreMoveInput(true);
        PlayerController->SetIgnoreLookInput(true);
        // 마우스 커서
        //PlayerController->bShowMouseCursor = true;
    }

    CurrentState = EEPLoadingState::LoadingAssets;

    // 이전에 로드된 스트리밍 레벨 언로드
    if (!PendingLevelName.IsNone() && PendingLevelName != LevelName)
    {
        FLatentActionInfo LatentInfo;
        UGameplayStatics::UnloadStreamLevel(this, PendingLevelName, LatentInfo, false);
    }

    PendingLevelName = LevelName;
    TargetPLevelEntranceTag = LevelEntranceTag;

    UAssetManager& AssetManager = UAssetManager::Get();
    
    // 기존 레벨 에셋 해제
    if (CurrentLoadedAssetId.IsValid())
    {
        // 이전 assetId 의 번들을 빈 배열로 세팅 (번들 상태 초기화)
        TArray<FName> EmptyBundles;
        AssetManager.ChangeBundleStateForPrimaryAssets({ CurrentLoadedAssetId }, EmptyBundles, EmptyBundles);

        CurrentLoadedAssetId = FPrimaryAssetId(); // 초기화
    }

    CurrentLoadedAssetId = AssetManager.GetPrimaryAssetIdForPath(AssetLabel.ToSoftObjectPath());

    // 유효성 체크
    if (!CurrentLoadedAssetId.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid Stage ID: %s | is not valid"), *CurrentLoadedAssetId.ToString());
        OnAssetsDone();
        return;
    }

    // 새로운 에셋 로드
    TArray<FName> Bundles;

    if (CurrentLoadedAssetId.IsValid())
    {
        // 비동기 로드 시작
        AssetLoadHandle = AssetManager.LoadPrimaryAsset(CurrentLoadedAssetId, Bundles, FStreamableDelegate::CreateUObject(this, &UEPLoadingSubsystem::OnAssetsDone));
        
        CheckProgress();
        
        // 다음 tick에 전달
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEPLoadingSubsystem::ProcessLoading);
    }
}

void UEPLoadingSubsystem::ProcessLoading()
{
    // 진행률 체크 타이머 시작 (0.05초 간격)
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_Progress, this, &UEPLoadingSubsystem::CheckProgress, 0.05f, true);
}

// 일부 로딩 상태 검사 (진행률 방송)
void UEPLoadingSubsystem::CheckProgress()
{
    float FinalProgress = 0.0f;
    UE_LOG(LogTemp, Warning, TEXT("[Loading ui] CheckProgress start"));

    if (CurrentState == EEPLoadingState::LoadingAssets)
    {
        if (!AssetLoadHandle.IsValid()) return;

        // 진행률 구간 : 0% ~ 40% 구간
        float AssetProgress = AssetLoadHandle->GetProgress();
        FinalProgress = AssetProgress * 0.4f;
        UE_LOG(LogTemp, Warning, TEXT("[Loading ui :: asset load] percent : %f"), FinalProgress);
    }
    else if (CurrentState == EEPLoadingState::LoadingFX)
    {
        // 진행률 구간 : 40% ~ 70% 구간
        UEPFXPreloadLibrary* FXSubsystem = GetGameInstance()->GetSubsystem<UEPFXPreloadLibrary>();
        float FXProgress = FXSubsystem->GetFXLoadProgress();

        FinalProgress = 0.4f + (FXProgress * 0.3f);
        UE_LOG(LogTemp, Warning, TEXT("[Loading ui :: fx load] percent : %f"), FinalProgress);
    }
    else if (CurrentState == EEPLoadingState::LoadingStreamLevel)
    {
        // 진행률 구간 : 70 ~ 100% 구간
        ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(this, PendingLevelName);
        if (Level)
        {
            // GetAsyncLoadPercentage는 0~100 사이의 정수를 반환하므로 100으로 나눕니다.
            float StreamProgress = FMath::Clamp(GetAsyncLoadPercentage(Level->GetWorldAssetPackageFName()) / 100.0f, 0.0f, 1.0f);
            FinalProgress = 0.7f + (StreamProgress * 0.3f);
            UE_LOG(LogTemp, Warning, TEXT("[Loading ui :: level load] percent : %f | stream porgress : %f"), FinalProgress, StreamProgress);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[Loading ui :: level load] percent is level fail"));
        }
    }

    // UI로 진행률 전달
    CurrentWidget->UpdateProgress(FinalProgress);
}

void UEPLoadingSubsystem::OnAssetsDone()
{
    // Asset loading 완료 -> FX loading 시작
    CurrentState = EEPLoadingState::LoadingFX;
    CheckProgress();

    UEPFXPreloadLibrary* FXSubsystem = GetGameInstance()->GetSubsystem<UEPFXPreloadLibrary>();
    // FX 로드가 끝나면 OnFXDone 호출되게 바인딩 필요
    FXSubsystem->OnPreloadCompleted.AddDynamic(this, &UEPLoadingSubsystem::OnFXLoadDone);

    FXSubsystem->PreloadFXForLevel(PendingLevelName); // FX 로드 요청
}

void UEPLoadingSubsystem::OnFXLoadDone()
{
    // FX loading 완료 -> Stream Level loading 시작
    CurrentState = EEPLoadingState::LoadingStreamLevel;
    CheckProgress();

    // 스트리밍 레벨 로드 시작
    OpenTargetLevel(PendingLevelName);
}

void UEPLoadingSubsystem::OpenTargetLevel(FName LevelName)
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSys)
    {
        // 네비게이션 빌드 잠시 중단 (서스펜드)
        NavSys->SetNavigationAutoUpdateEnabled(false, NavSys);
    }

    UE_LOG(LogTemp, Warning, TEXT("[ERROR 1] target level open start function"));
    // Latent Action 설정 (콜백 연결)
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;             // 누가 콜백을 받을지
    LatentInfo.ExecutionFunction = "OnAllFinished"; // 호출할 함수 이름
    LatentInfo.UUID = 12345;                      // 고유 ID (아무 정수나 겹치지 않게)
    LatentInfo.Linkage = 0;

    // 로드 시작
    // bMakeVisibleAfterLoad: true면 로드 되자마자 화면에 보임 (Open)
    // bShouldBlockOnLoad: false (비동기)
    UGameplayStatics::LoadStreamLevel(this, LevelName, true, false, LatentInfo);
}

// 전체 로딩 완료
void UEPLoadingSubsystem::OnAllFinished()
{
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Progress);

    if (CurrentWidget)
    {
        CurrentWidget->UpdateProgress(1.0f);// 100% 확정 (widget -> fade out anim play)
    }

}

// 최종 레벨 open
void UEPLoadingSubsystem::OnLevelDataLoaded()
{
    UE_LOG(LogTemp, Warning, TEXT("Level Loaded and Visible!"));

    // 레벨을 실제로 보이게 설정
    ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(this, PendingLevelName);
    if (Level)
    {
        Level->SetShouldBeVisible(true);
    }

    // 플레이어 스폰 위치 조정
    SetupPlayerPosition();

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSys)
    {
        // 네비게이션 빌드 재개 및 강제 업데이트
        NavSys->SetNavigationAutoUpdateEnabled(true, NavSys);
        NavSys->Build();
    }

    // 스포너에 스폰 요청
    PrepareSpawnersBeforeFadeIn();
}

// 실제 level open 함수
void UEPLoadingSubsystem::CallOpenTargetLevel()
{
    UGameplayStatics::GetStreamingLevel(this, PendingLevelName)->SetShouldBeVisible(true);
}

// 월드 내 PlayerStart 반환 함수
AActor* UEPLoadingSubsystem::FindPlayerStartByTag(FName TagName)
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    // 월드 내 LevelEntrance 검색
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(World, AEPLevelEntrance::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        AEPLevelEntrance* LevelEntrance = Cast<AEPLevelEntrance>(Actor);
        if (LevelEntrance)
        {
            if (LevelEntrance->ActorHasTag(TargetPLevelEntranceTag))
            {
                // 액터의 소유 레벨 이름이 현재 로드 중인 레벨과 일치하는지 확인
                FString LevelName = LevelEntrance->GetLevel()->GetOuter()->GetName(); // 찾은 startpoinrt가 속한 level
                if (LevelName.Contains(PendingLevelName.ToString())) // 로드 중인 레벨과 일치 확인
                {
                    // 스폰 포인트 설정
                    if (UGameInstance* GameInstance = GetGameInstance())
                    {
                        if (UEPRespawnSubsystem* RespawnSubsystem = GameInstance->GetSubsystem<UEPRespawnSubsystem>())
                        {
                            RespawnSubsystem->SetRespawnPoint(LevelEntrance->GetActorTransform());
                        }
                    }

                    return LevelEntrance;
                }
            }
        }
    }

    // 못 찾았을 때 실패 처리
    UE_LOG(LogTemp, Error, TEXT("[Loading] Critical Error: Cannot find checkpoint with tag %s in level %s"), *TargetPLevelEntranceTag.ToString(), *PendingLevelName.ToString());
    return nullptr;
}

// 플레이어의 스폰 위치 조정
void UEPLoadingSubsystem::SetupPlayerPosition()
{
    // StartMap에 배치해둔 PlayerStart 찾기
    AActor* StartSpot = FindPlayerStartByTag(TargetPLevelEntranceTag);
    
    // 못 찾으면 0,0,0 사용
    FVector SpawnLoc = StartSpot ? StartSpot->GetActorLocation() : FVector::ZeroVector;
    FRotator SpawnRot = StartSpot ? StartSpot->GetActorRotation() : FRotator::ZeroRotator;
    UE_LOG(LogTemp, Warning, TEXT("[ERROR STARTPOINT] player spawn point = x : %f , y : %f, z : %f"), SpawnLoc.X, SpawnLoc.Y, SpawnLoc.Z);

    UWorld* World = GetWorld();

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (PlayerController && PlayerController->GetPawn())
    {
        PlayerController->GetPawn()->SetActorLocationAndRotation(SpawnLoc, SpawnRot);
    }
    else
    {
        // 새로운 폰 생성
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        // 새 캐릭터 스폰
        APawn* NewHero = World->SpawnActor<APawn>(PlayerPawnClass, SpawnLoc, SpawnRot, SpawnParams);

        // 빙의
        if (NewHero)
        {
            PlayerController->Possess(NewHero);
        }
    }
}

// ============================ UI ============================
// 로딩 위젯의 fade in anim 플레이 및 제거
void UEPLoadingSubsystem::PlayFadeIn()
{
    FTimerHandle WaitHandle;
    GetWorld()->GetTimerManager().SetTimer(WaitHandle, [this]()
        {
            if (CurrentWidget)
            {
                CurrentWidget->PlayFadeInAnimation(); // fade In anim 실행
            }

            // 사운드 믹스 제거
            RestoreWorldAudio(1.0f);

        }, 0.2f, false); // 0.2초의 안정화 시간 확보
}

// 위젯 생성 및 초기화
void UEPLoadingSubsystem::ShowLoadingScreen()
{
    // 이미 떠 있으면 무시
    if (CurrentWidget && CurrentWidget->IsInViewport())
        return;

    if (!LoadingWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("LoadingWidgetClass is not set in GameInstance!"));
        return;
    }

    if (!CurrentWidget)
    {
        CurrentWidget = CreateWidget<UEPLoadingWidget>(GetWorld(), LoadingWidgetClass);
    }

    // 위젯 생성
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        if (CurrentWidget)
        {
            CurrentWidget->AddToViewport(9999); // Z-Order 최상단
            CurrentWidget->SetVisibility(ESlateVisibility::Visible);

            CurrentWidget->InitializeWidgetState(); // 여기서 진행률 0으로 초기화 및 바인딩 점검
            CurrentWidget->OnFadeOutComplete.RemoveDynamic(this, &UEPLoadingSubsystem::HandleFadeOutFinished); // 제거
            CurrentWidget->OnFadeOutComplete.AddDynamic(this, &UEPLoadingSubsystem::HandleFadeOutFinished); // 바인딩

            CurrentWidget->OnFadeInComplete.RemoveDynamic(this, &UEPLoadingSubsystem::HandleFadeInFinished); // 제거
            CurrentWidget->OnFadeInComplete.AddDynamic(this, &UEPLoadingSubsystem::HandleFadeInFinished); // 바인딩
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("[LOAIDNG UI] widget is new create"));
}

void UEPLoadingSubsystem::HideLoadingScreen()
{
    if (CurrentWidget && CurrentWidget->IsInViewport())
    {
        CurrentWidget->RemoveFromParent();
    }

    // 클래스 기반으로 한 번 더 확인 (방어적 코드)
    if (UWorld* World = GetWorld())
    {
        TArray<UUserWidget*> FoundWidgets;
        UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, FoundWidgets, LoadingWidgetClass, false);

        for (UUserWidget* Widget : FoundWidgets)
        {
            Widget->RemoveFromParent();
        }
    }
}

// fade out 종료시 호출 함수
void UEPLoadingSubsystem::HandleFadeOutFinished()
{
    // 로드된 레벨 오픈 및 추가 처리 (spawn 위치 조정, fade in anim 플레이)
    OnLevelDataLoaded();
}

// fade in 종료시 호출 함수
void UEPLoadingSubsystem::HandleFadeInFinished()
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->SetIgnoreMoveInput(false);
        PlayerController->SetIgnoreLookInput(false);
        // 마우스 커서
        FInputModeGameOnly InputMode;
        PlayerController->SetInputMode(InputMode);
    }

    HideLoadingScreen();

    // 맵에 있는 모든 EPMonsterSpawner를 다시 찾아서 일제히 "기상 타이머 시작!" 명령을 내립니다.
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEPMonsterSpawner::StaticClass(), FoundSpawners);

    for (AActor* Actor : FoundSpawners)
    {
        if (AEPMonsterSpawner* Spawner = Cast<AEPMonsterSpawner>(Actor))
        {
            // 이 명령을 받으면 스포너들은 자신의 SpawnDelayTime (0.5초 등)을 돌리고 몬스터를 짠 나타나게 합니다.
            Spawner->StartWakeUpSequence();
        }
    }
}

// ============================ AUDIO ============================
// 월드 사운드 제거
void UEPLoadingSubsystem::MuteWorldAudio()
{
    if (LoadingSoundMix && GetWorld())
    {
        // 로딩 믹스 적용 (게임 소리 즉시 차단)
        UGameplayStatics::PushSoundMixModifier(GetWorld(), LoadingSoundMix);
    }
}

// 월드 사운드 적용
void UEPLoadingSubsystem::RestoreWorldAudio(float FadeInTime)
{
    if (LoadingSoundMix && WorldSoundClass && GetWorld())
    {
        // 지정된 시간 동안 서서히 원래 볼륨으로 복구
        UGameplayStatics::SetSoundMixClassOverride(
            GetWorld(),
            LoadingSoundMix,
            WorldSoundClass,
            0.0f,  // 원래 볼륨
            1.0f,  // 기본 피치
            FadeInTime,
            true
        );

        FTimerHandle PopWaitHandle;
        GetWorld()->GetTimerManager().SetTimer(PopWaitHandle, [this]()
            {
                // 로딩 믹스 제거
                UGameplayStatics::PopSoundMixModifier(GetWorld(), LoadingSoundMix);
            }, FadeInTime, false); // FadeInTime 이후 확실히 정리

    }
}

// ============================ SPAWNER ============================
// 스포너에 스폰 요청 및 바인딩 (맵 로딩 & 리스폰 세팅이 끝난 직후)
void UEPLoadingSubsystem::PrepareSpawnersBeforeFadeIn()
{
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEPMonsterSpawner::StaticClass(), FoundSpawners);

    PendingSpawnersCount = 0;

    for (AActor* Actor : FoundSpawners)
    {
        if (AEPMonsterSpawner* Spawner = Cast<AEPMonsterSpawner>(Actor))
        {
            // 로딩에 스폰될 스포너인지 확인
            if (Spawner->GetbSpawnOnBeginPlay())
            {
                PendingSpawnersCount++;

                // 스포너의 준비 완료 신호 바인딩
                Spawner->OnSpawnerReady.AddDynamic(this, &UEPLoadingSubsystem::HandleSingleSpawnerReady);
                Spawner->RequestSpawn(); // 스폰 요청
            }
        }
    }

    // 로딩에 스폰할 스포너가 없을 경우
    if (PendingSpawnersCount == 0)
    {
        PlayFadeIn(); // fade in 플레이
    }
}

// 스포너가 스폰 성공했을 때마다 호출
void UEPLoadingSubsystem::HandleSingleSpawnerReady()
{
    PendingSpawnersCount--;

    // 맵 안의 모든 필수 스포너 준비 완료되었을 때
    if (PendingSpawnersCount <= 0)
    {
        PlayFadeIn(); // fade in 플레이
    }
}
