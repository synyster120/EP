// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Helper/EPItemLibrary.h"

// DroppedItem 상속받은 드랍 전용 Actor 생성 및 초기화하는 헬퍼 함수
AEPDroppedItem* UEPItemLibrary::SpawnDroppedItem(UObject* WorldContextObject, TSubclassOf<AEPDroppedItem> DroppedItemClass, TSubclassOf<AActor> OriginalItemClass, FVector SpawnLocation, int32 Quantity)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World || !DroppedItemClass || !OriginalItemClass) return nullptr;

    // 랜덤 회전 등 스폰 파라미터 설정
    FRotator SpawnRotation = FRotator::ZeroRotator;
    SpawnRotation.Yaw = FMath::FRandRange(0.0f, 360.0f);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // 실제 스폰
    AEPDroppedItem* DroppedActor = World->SpawnActor<AEPDroppedItem>(
        DroppedItemClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

    // 데이터 주입 
    // *** 물리는 별도로 각자 적용 해줘야함 ***
    if (DroppedActor)
    {
        DroppedActor->InitializeDrop(OriginalItemClass, Quantity);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnDroppedItem: OriginalItemClass is NULL!"));
    }

    return DroppedActor;
}
