// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Subsystems/EPObjectPoolManager.h"
#include "Core/Interfaces/EPPoolable.h"

// 게임 시작 시 필요한 초기화 로직 (틀만 존재-필요하면 추가)
void UEPObjectPoolManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

// 게임 종료 시, 메모리 누수를 막기 위해 모든 투사체를 파괴
void UEPObjectPoolManager::Deinitialize()
{
    for (auto& PoolPair : Pools)
    {
        for (AActor* NewActor : PoolPair.Value.PooledActor)
        {
            if (IsValid(NewActor))
            {
                NewActor->Destroy();
            }
        }
    }
    Pools.Empty();
    Super::Deinitialize();
}

AActor* UEPObjectPoolManager::SpawnObjectFromPool(TSoftClassPtr<AActor> ActorClass, const FTransform& SpawnTransform, const FEPPoolableObjectInitializer& Initializer)
{
    // 풀에서 비활성화된 객체를 빌려옴
    AActor* PooledActor = GetObjectFromPool(ActorClass);
    if (PooledActor)
    {
        // 위치 설정
        PooledActor->SetActorTransform(SpawnTransform);

        // 객체 데이터 초기화
        if (IEPPoolable* PoolableActor = Cast<IEPPoolable>(PooledActor))
        {
            // 표준화된 데이터를 전달하여 초기화를 위임
            //PoolableActor->Execute_PoolableInitialize(PooledActor, Initializer);
            //PoolableActor->poo
            //// 활성화 명령
            //PoolableActor->Execute_PoolableActivate(PooledActor);
        }
    }
    return PooledActor;
}

// Pool 가져오기 (실패 시 최초 생성, 동적 확장)
AActor* UEPObjectPoolManager::GetObjectFromPool(TSoftClassPtr<AActor> ActorClass)
{
    // 인터페이스 구현했는지 확인
    if (!ActorClass.LoadSynchronous()->ImplementsInterface(UEPPoolable::StaticClass()))
    {
        UE_LOG(LogTemp, Error, TEXT("Tried to get an object from pool with class %s which does not implement IEPPoolable!"), *ActorClass.ToString());
        return nullptr;
    }

    if (!ActorClass) return nullptr;

    // 해당 클래스를 위한 풀이 존재하는지 확인 후 없다면 새로 생성
    FEPObjectPool& Pool = Pools.FindOrAdd(ActorClass);
    if (Pool.PooledActor.Num() == 0) // 최초 생성 시
    {
        CreatePoolForClass(ActorClass, Pool); // 풀을 채우도록 수정
    }

    // 해당 풀에서 비활성화된(사용 가능한) 투사체를 찾음
    for (AActor* PoolActor : Pool.PooledActor)
    {
        if (Cast<IEPPoolable>(PoolActor)->IsActive() == false)// 비활성화 상태인지 확인
        {
            return PoolActor; // 찾았으면 즉시 반환
        }
    }

    // 만약 모든 투사체가 사용 중이라면 (동적 확장)
    // .LoadSynchronous()를 호출하여 TSoftClassPtr에서 UClass*를 가져옴
    UClass* ClassToSpawn = ActorClass.LoadSynchronous();
    if (ClassToSpawn)
    {
        AActor* NewSpawnActor = GetWorld()->SpawnActor<AActor>(ClassToSpawn);
        
        if(NewSpawnActor)
        {
            // 스폰된 액터를 IPoolable 인터페이스로 Cast (Poolable 인터페이스 소유 확인)
            if (IEPPoolable* PoolableActor = Cast<IEPPoolable>(NewSpawnActor))
            {
                PoolableActor->SetOwnerPool(this); // 자신이 속한 풀을 알려줌
                PoolableActor->Deactivate(); // 스폰 직후 즉시 비활성화

                Pool.PooledActor.Add(NewSpawnActor); // 기존 풀에 '한 번만' 추가
                return NewSpawnActor; // 새로 만든 객체를 반환
            }
            else
            {
                // Cast에 실패했다면, 풀에서 사용하면 안되는 액터이므로 경고 로그를 남기고 즉시 파괴
                UE_LOG(LogTemp, Warning, TEXT("Actor %s was spawned by the pool manager but does not implement IEPPoolable interface! Destroying actor."), *NewSpawnActor->GetName());
                NewSpawnActor->Destroy();
            }
        }
    }

    return nullptr; // 스폰 최종 실패 시
}

// 투사체가 자신을 풀에 반납하기 위해 호출하는 함수
void UEPObjectPoolManager::ReturnObjectToPool(AActor* ReturnActor)
{
    if (ReturnActor)
    {
        // 투사체에게 소멸 준비를 시작하라고 통지 후,
        // 소멸 이펙트 등에 필요한 지연 시간을 반환받음

        if (IEPPoolable* PoolableActor = Cast<IEPPoolable>(ReturnActor))
        {
            const float DeactivationDelay = PoolableActor->BeginDeactivate();

            // 만약 지연 시간이 필요하다면
            if (DeactivationDelay > 0.f)
            {
                // FTimerDelegate를 사용하여 파라미터가 있는 함수를 호출하도록 예약
                FTimerHandle DelayTimerHandle;

                // 람다를 사용하여 타이머가 끝났을 때 실행될 로직을 바로 정의합니다.
                GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, [this, ReturnActor]()
                    {
                        // 람다 내부에서는 캡처한 변수를 안전하게 사용하는 것이 좋습니다.
                        if (IsValid(ReturnActor))
                        {
                            FinalizeDeactivation(ReturnActor);
                        }
                    }, DeactivationDelay, false);
            }
            else
            {
                // 지연이 필요 없다면 즉시 최종 비활성화 실행
                FinalizeDeactivation(ReturnActor);
            }
        }

    }
}

// 최종적으로 투사체 비활성화하는 함수 (ReturnObjectToPool 에서 호출)
void UEPObjectPoolManager::FinalizeDeactivation(AActor* ReturnActor)
{
    if (ReturnActor)
    {
        if (IEPPoolable* PoolableActor = Cast<IEPPoolable>(ReturnActor))
        {
            // "비활성화" 명령
            PoolableActor->Deactivate();
        }
    }
}

// 특정 클래스의 풀을 처음으로 생성하는 내부 함수
void UEPObjectPoolManager::CreatePoolForClass(TSoftClassPtr<AActor> ActorClass, FEPObjectPool& PoolToFill)
{
    // 인터페이스 구현했는지 확인
    if (!ActorClass.LoadSynchronous()->ImplementsInterface(UEPPoolable::StaticClass()))
    {
        UE_LOG(LogTemp, Error, TEXT("Tried to get an object from pool with class %s which does not implement IEPPoolable!"), *ActorClass.ToString());
        return;
    }

    // InitialPoolSizes 맵에 해당 클래스에 대한 설정값이 있는지 찾아보고, 없으면 기본값을 사용합니다.
    const int32 Size = InitialPoolSizes.Contains(ActorClass) ? InitialPoolSizes[ActorClass] : DefaultInitialPoolSize;

    for (int32 i = 0; i < Size; ++i)
    {
        // .LoadSynchronous()를 호출하여 TSoftClassPtr에서 UClass*를 가져옴
        UClass* ClassToSpawn = ActorClass.LoadSynchronous();
        AActor* NewActor = GetWorld()->SpawnActor<AActor>(ClassToSpawn);
        if (NewActor)
        {
            if (IEPPoolable* PoolableActor = Cast<IEPPoolable>(NewActor))
            {
                PoolableActor->SetOwnerPool(this);
                PoolableActor->Deactivate();
                PoolToFill.PooledActor.Add(NewActor);
            }
        }
    }
}
