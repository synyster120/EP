// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EPObjectPoolManager.generated.h"

struct FEPPoolableObjectInitializer;
/**
 * 
 */
 
 // 단일 종류의 투사체를 관리하는 풀 구조체
USTRUCT()
struct FEPObjectPool
{
    GENERATED_BODY()

    // 이 풀에 속한 모든 투사체 객체들
    UPROPERTY()
    TArray<TObjectPtr<AActor>> PooledActor; // 이미 월드에 생성되어 메모리에 올라와 있는 객체를 안전하게 참조(TObjectPtr)
};

// 편집(Edit) → 프로젝트 세팅(Project Settings) 에서 InitialPoolSizes 값 수정 가능
UCLASS(config=Game)
class STARBOARDMAP_API UEPObjectPoolManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    // 서브시스템이 생성/파괴될 때 호출되는 함수들
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // 최종적인 스폰 함수. 표준 초기화 데이터를 받음
    AActor* SpawnObjectFromPool(TSoftClassPtr<AActor> ActorClass, const FTransform& SpawnTransform, const FEPPoolableObjectInitializer& Initializer);

    // 외부에서 투사체를 빌려가기 위해 호출하는 메인 함수
    AActor* GetObjectFromPool(TSoftClassPtr<AActor> ActorClass);

    // 투사체가 자신을 풀에 반납하기 위해 호출하는 함수
    void ReturnObjectToPool(AActor* ReturnActor);

protected:
    // 투사체 클래스별 초기 풀 사이즈를 지정하는 맵.
    // 프로젝트 세팅이나 DefaultGame.ini 파일에서 이 값을 쉽게 수정할 수 있습니다.
    UPROPERTY(Config, EditDefaultsOnly, Category = "Pooling", meta = (IsImplementing = "EPPoolable"))
    TMap<TSoftClassPtr<AActor>, int32> InitialPoolSizes;

    // 만약 위 맵에 지정되지 않은 투사체일 경우 사용할 기본값
    UPROPERTY(Config, EditDefaultsOnly, Category = "Pooling")
    int32 DefaultInitialPoolSize = 10;

    // 투사체 클래스별로 개별적인 풀을 관리하는 TMap
    UPROPERTY()
    TMap<TSoftClassPtr<AActor>, FEPObjectPool> Pools;

    // 최종적으로 투사체 비활성화하는 함수 (ReturnObjectToPool 에서 호출)
    void FinalizeDeactivation(AActor* ReturnActor);

private:
    // 특정 클래스의 풀을 처음으로 생성하는 내부 함수
    void CreatePoolForClass(TSoftClassPtr<AActor> ActorClass, FEPObjectPool& PoolToFill);
};
