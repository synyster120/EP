
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EPMonsterSpawner.generated.h"

class UBoxComponent;
class AEPEnemyCharacter;

/*
**      딱 한 번의 스폰(One-Shot)을 보장하는 Spawner - 외부에서 스폰 요청/동결 해제 가능
*/

// loading subsystem이 수신받을 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawnerReadySignature);

UENUM(BlueprintType)
enum class EEPSpawnerState : uint8
{
    Idle                UMETA(DisplayName = "대기 중"),
    CheckingNavMesh     UMETA(DisplayName = "네비메시 확인 중"), 
    Hibernating         UMETA(DisplayName = "동면 중 (서브시스템의 기상 신호 대기)"),
    WaitingForDelay     UMETA(DisplayName = "기상 신호받고 스폰 딜레이 대기 중 (연출용 딜레이 카운트다운 중)"),
    Finished            UMETA(DisplayName = "스폰 완료됨")
};

UCLASS()
class STARBOARDMAP_API AEPMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEPMonsterSpawner();

    // loading subsystem이 스포너의 준비 완료를 듣기 위한 델리게이트
    UPROPERTY(BlueprintAssignable)
    FOnSpawnerReadySignature OnSpawnerReady;

protected:
	virtual void BeginPlay() override; 
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // nav mesh 유효한지 확인후 스폰 시작 함수
    UFUNCTION()
    void CheckAndExecuteSpawn();

    UFUNCTION()
    void ExecuteSpawn();

    void FreezeMonsters();
    void WakeUpMonsters();

public:
    // 외부에서 호출하는 스폰 함수
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void RequestSpawn();

    // 동결 상태 해제 함수
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void StartWakeUpSequence();

    // getter/setter
    FORCEINLINE bool GetbSpawnOnBeginPlay() { return bSpawnOnBeginPlay; };
    FORCEINLINE void SstSpawnDelayTime(float CurrentSpawnDelayTime) { SpawnDelayTime = CurrentSpawnDelayTime; };

protected:
    // 스폰 상태 Enum
    UPROPERTY(VisibleAnywhere, Category = "Spawning")
    EEPSpawnerState CurrentState = EEPSpawnerState::Idle;

    // 스폰 범위
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* SpawnArea;

    // 스폰할 enemy 블루프린트 클래스 지정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<AEPEnemyCharacter> EnemyClassToSpawn;

    // 한 번에 스폰할 몬스터 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    int32 SpawnCount = 1;

    // 로딩 중 스폰할지 여부
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    bool bSpawnOnBeginPlay = false;

    // 스폰 이후 동결 해제 딜레이 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    float SpawnDelayTime = 0.0f;

    // 생성된 몬스터들을 동면/기상시키기 위해 기억해두는 배열
    UPROPERTY()
    TArray<AEPEnemyCharacter*> SpawnedMonsters;

    // 타이머 관리를 위한 핸들
    FTimerHandle NavCheckTimerHandle; // 스폰 체크
    FTimerHandle WakeUpDelayTimerHandle; // 동결 해제
};
