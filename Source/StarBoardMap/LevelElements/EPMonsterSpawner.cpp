
#include "LevelElements/EPMonsterSpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/EPEnemyCharacter.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AEPMonsterSpawner::AEPMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	RootComponent = SpawnArea;
	SpawnArea->SetBoxExtent(FVector(500.f, 500.f, 100.f));
	SpawnArea->SetCollisionProfileName(TEXT("NoCollision")); // 충돌x 영역 표시용
}

void AEPMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

}

// 자체적 nav mesh 체크 후 스폰 함수
void AEPMonsterSpawner::CheckAndExecuteSpawn()
{
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    // 실제 nav mesh 바닥 데이터(MainNavData)가 월드에 로드 되었는지 확인
    ANavigationData* NavData = NavSystem ? NavSystem->GetDefaultNavDataInstance() : nullptr;

    // 네비메시 빌드가 진행 중이라면(안 끝났다면) 다음 timer에 스폰
    if (NavSystem && NavData &&!NavSystem->IsNavigationBuildInProgress())
    {
        // 빌드가 완전히 끝났을 때 loop timer 제거
        GetWorld()->GetTimerManager().ClearTimer(NavCheckTimerHandle);

        // 스폰
        ExecuteSpawn();
    }
}

// 외부에서 호출하는 스폰 함수
void AEPMonsterSpawner::RequestSpawn()
{
    if (CurrentState != EEPSpawnerState::Idle) return;

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

    // 실제 nav mesh 바닥 데이터(MainNavData)가 월드에 로드 되었는지 확인
    ANavigationData* NavData = NavSystem ? NavSystem->GetDefaultNavDataInstance() : nullptr;

    // 네비메시 빌드가 준비되었는지 확인
    if (NavSystem && NavData && !NavSystem->IsNavigationBuildInProgress())
    {
        // 스폰
        ExecuteSpawn();
    }
    else
    {
        CurrentState = EEPSpawnerState::CheckingNavMesh; // 상태 변경
        // nav mesh 자체적 검사 후 스폰 timer 사용
        GetWorld()->GetTimerManager().SetTimer(NavCheckTimerHandle, this, &AEPMonsterSpawner::CheckAndExecuteSpawn, 0.1f, true);
    }
}

// 서브시스템이 FadeIn 끝난 후 호출함
void AEPMonsterSpawner::StartWakeUpSequence()
{
    if (CurrentState != EEPSpawnerState::Hibernating) return;

    // 💡 누락된 부분 추가: 이제 딜레이 대기 상태로 진입함을 명시!
    CurrentState = EEPSpawnerState::WaitingForDelay;

    if (SpawnDelayTime > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(WakeUpDelayTimerHandle, this, &AEPMonsterSpawner::WakeUpMonsters, SpawnDelayTime, false);
    }
    else
    {
        WakeUpMonsters();
    }
}

// 실제 스폰 로직
void AEPMonsterSpawner::ExecuteSpawn()
{
    if (!EnemyClassToSpawn || !GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("Spawner %s has no EnemyClassToSpawn!"), *GetName());
        CurrentState = EEPSpawnerState::Finished;
        return;
    }

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSystem) 
    {
        UE_LOG(LogTemp, Error, TEXT("Spawner %s has no NavSystem!"), *GetName());
        CurrentState = EEPSpawnerState::Finished;
        return;
    }


    int32 CurrentSpawnedCount = 0;
    int32 AttemptCount = 0;
    int32 MaxAttempts = SpawnCount * 5; // 목표량의 5배까지만 재시도를 허용 (무한 루프 방지)

    // 목표한 스폰 마리 수를 채우거나, 최대 시도 횟수를 초과할 때까지 반복
    while (CurrentSpawnedCount < SpawnCount && AttemptCount < MaxAttempts)
    {
        AttemptCount++; // 시도 횟수 증가

        // 박스 영역에서 랜덤 위치
        FVector RawRandomLocation = UKismetMathLibrary::RandomPointInBoundingBox(SpawnArea->Bounds.Origin, SpawnArea->Bounds.BoxExtent);

        // line trace 의시작점(박스의 천장) & 끝점(박스의 바닥) 계산
        FVector TraceStart = FVector(RawRandomLocation.X, RawRandomLocation.Y, SpawnArea->Bounds.Origin.Z + SpawnArea->Bounds.BoxExtent.Z);
        FVector TraceEnd = FVector(RawRandomLocation.X, RawRandomLocation.Y, SpawnArea->Bounds.Origin.Z - SpawnArea->Bounds.BoxExtent.Z);

        FHitResult HitResult;
        FCollisionQueryParams TraceParams;
        TraceParams.AddIgnoredActor(this); // 스포너 충돌 무시
        
        // line trace
        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, TraceParams);

        if (bHit) // 바닥 존재 여부
        {
            FNavLocation ValidNavLocation;
            FVector QueryExtent(50.f, 50.f, 50.f); // 탐색용 투명 박스의 크기(반경)

            // Nav Mesh 존재 여부
            if (NavSystem->ProjectPointToNavigation(HitResult.Location, ValidNavLocation, QueryExtent)) // 파라미터 : 기준점, 결과물, 검색 반경
            {
                FRotator RandomRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);
                FActorSpawnParameters SpawnParams;

                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding; // 겹치면 밀어내되, 실패하면 스폰 취소

                // 묻힘 방지 (표면에 10.f 띄워 스폰)
                FVector FinalSpawnLocation = ValidNavLocation.Location + FVector(0.f, 0.f, 10.f);

                AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(EnemyClassToSpawn, FinalSpawnLocation, RandomRotation, SpawnParams);

                if (AEPEnemyCharacter* Enemy = Cast<AEPEnemyCharacter>(SpawnedActor))
                {
                    // 스폰 성공
                    SpawnedMonsters.Add(Enemy);
                    CurrentSpawnedCount++;
                }
            }
        }
        else
        {
            // 스폰 실패
        }
    }

    if (CurrentSpawnedCount < SpawnCount)
    {
        // 에디터 배치 실수(NavMesh 부족) 경고
        UE_LOG(LogTemp, Warning, TEXT("Spawner %s failed to spawn all enemies. Spawned %d / %d"), *GetName(), CurrentSpawnedCount, SpawnCount);
    }

    // 투사(Project) 실패로 한 마리도 생성하지 못할 경우
    if (CurrentSpawnedCount == 0 && SpawnCount > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("NavMesh loading delayed. Retrying in 0.3s..."));

        CurrentState = EEPSpawnerState::CheckingNavMesh; // 상태 변경

        // 0.5초 뒤에 다시 시도
        GetWorld()->GetTimerManager().SetTimer(NavCheckTimerHandle, this, &AEPMonsterSpawner::CheckAndExecuteSpawn, 0.3f, true);

        return;
    }

    // 동면상태로 변경
    FreezeMonsters();

    CurrentState = EEPSpawnerState::Hibernating; // 상태 변경

    if (bSpawnOnBeginPlay)
    {
        // loading subsystem에 스폰 완료&동면 상태(준비 끝) 방송
        OnSpawnerReady.Broadcast();
    }
    else
    {
        StartWakeUpSequence();
    }
}

void AEPMonsterSpawner::FreezeMonsters()
{
    // AI 비활성화
    for (AEPEnemyCharacter* Monster : SpawnedMonsters)
    {
        if (!Monster) continue;

        Monster->SetActorHiddenInGame(true);
        Monster->SetActorEnableCollision(false);

        if (AAIController* AI = Cast<AAIController>(Monster->GetController()))
        {
            if (UBrainComponent* Brain = AI->GetBrainComponent())
            {
                Brain->StopLogic("FrozenForLoading");
            }
        }
    }
}

void AEPMonsterSpawner::WakeUpMonsters()
{
    if (CurrentState != EEPSpawnerState::WaitingForDelay) return;

    for (AEPEnemyCharacter* Monster : SpawnedMonsters)
    {
        if (!Monster) continue;

        Monster->SetActorHiddenInGame(false);
        Monster->SetActorEnableCollision(true);

        if (AAIController* AI = Cast<AAIController>(Monster->GetController()))
        {
            if (UBrainComponent* Brain = AI->GetBrainComponent())
            {
                Brain->RestartLogic();
            }
        }
    }

    SpawnedMonsters.Empty(); // 메모리 정리
    CurrentState = EEPSpawnerState::Finished;
}

void AEPMonsterSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 모든 timer 정리
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

    Super::EndPlay(EndPlayReason);
}


