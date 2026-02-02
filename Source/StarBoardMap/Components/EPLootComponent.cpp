
#include "Components/EPLootComponent.h"
#include "Components/EPStatComponent.h"
#include "Data/EPItemTypes.h"
#include "Core/Helper/EPAsyncLoadHelper.h"
#include "GameFramework/Character.h"
#include "Items/EPDroppedItem.h"
#include "Items/EPItemBase.h"
#include "Core/Helper/EPItemLibrary.h"

UEPLootComponent::UEPLootComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UEPLootComponent::BeginPlay()
{
	Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (Owner)
    {
        // Owner의 StatComponent를 찾음 (의존성)
        auto* StatComp = Owner->FindComponentByClass<UEPStatComponent>();
        if (StatComp)
        {
            // 죽음 이벤트에 Dropt 함수 바인딩
            StatComp->OnDied.AddDynamic(this, &UEPLootComponent::SpawnLoot);
        }
    }
}

// 스폰 개수 계산 및 반환
int32 UEPLootComponent::GetQuantityToSpawn(const TArray<FEPItemQuantityRange>& Ranges)
{
    // 데이터가 비어있으면 0개 리턴
    if (Ranges.Num() == 0) return 0;

    // 가중치 총합 구하기 (Total Weight Calculation)
    float TotalWeight = 0.0f;
    for (const auto& Entry : Ranges)
    {
        TotalWeight += Entry.Weight;
    }

    // 가중치 합이 0이면 0개 리턴
    if (TotalWeight <= 0.0f) return 0;

    // 랜덤 포인트 선정 (0 ~ 총합 사이)
    float RandomPoint = FMath::FRandRange(0.0f, TotalWeight);

    // 어떤 구간에 걸렸는지 확인 (Weighted Selection Algorithm)
    for (const auto& Entry : Ranges)
    {
        // 랜덤 포인트가 현재 가중치보다 작으면 스폰 결정
        if (RandomPoint <= Entry.Weight)
        {
            // 이 구간의 최소~최대 사이에서 랜덤 개수 결정 후 반환
            return FMath::RandRange(Entry.MinQuantity, Entry.MaxQuantity);
        }

        // 당첨 안 됐으면, 랜덤 포인트에서 현재 가중치만큼 뺌 (다음 구간 확인을 위해)
        RandomPoint -= Entry.Weight;
    }

    return 0; // 로직상 여기까지 올 일은 없지만 안전장치
}

// Item Drop 함수(Drop 시작)
void UEPLootComponent::SpawnLoot()
{
    // Drop 데이터 테이블 핸들 유효성 검사
    if (LootDataTableHandle.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("LootDataTableHandle is null in %s"), *GetOwner()->GetName());
        return;
    }

    // 테이블에서 행(Row) 가져오기
    FEPDropTable* DropTable = LootDataTableHandle.GetRow<FEPDropTable>(TEXT("LootContext"));

    if (!DropTable) return;

    // 드랍 아이템 목록 순회
    for (const FEPDropItemInfo& ItemInfo : DropTable->DropItems)
    {
        // 확률 계산 후 드랍할 개수 반환
        int32 SpawnQuantity = GetQuantityToSpawn(ItemInfo.DropRanges);

        if (SpawnQuantity > 0)
        {
            // 드랍 처리
            ProcessDropItem(ItemInfo, SpawnQuantity);
        }
    }

}

// 아이템 드랍
void UEPLootComponent::ProcessDropItem(const FEPDropItemInfo& DropItemInfo, const int32 CurrentQuantity)
{
    //  중첩된 테이블 핸들(DropItemRow)에서 실제 아이템 클래스 찾기
    if (DropItemInfo.DropItemRow.IsNull()) return;

    // FEPItemInfo는 아이템 DB의 구조체라고 가정합니다.
    FEPItemData* ItemData = DropItemInfo.DropItemRow.GetRow<FEPItemData>(TEXT("ItemContext"));

    if (!ItemData || ItemData->SoftItemClass.IsNull()) return;

    // 개수만큼 드랍
    for (int32 i = 0; i < CurrentQuantity; ++i)
    {
        // 스폰 위치 계산 (바닥에 묻히지 않게, 약간 랜덤하게)
        FVector SpawnLoc = GetOwner()->GetActorLocation();
        SpawnLoc.Z += 20.0f; // 높이 보정
        SpawnLoc.X += FMath::FRandRange(-50.0f, 50.0f); // 랜덤 분산
        SpawnLoc.Y += FMath::FRandRange(-50.0f, 50.0f);

        FRotator SpawnRot = FRotator::ZeroRotator;
        SpawnRot.Yaw = FMath::FRandRange(0.0f, 360.0f); // 랜덤 회전

        // 데이터 테이블에서 Soft Pointer 가져오기
        TSoftClassPtr<AActor> SoftItemClass = ItemData->SoftItemClass;

        // 이미 로드되어 있는지 확인 (캐시)
        if (UClass* LoadedClass = SoftItemClass.Get())
        {
            SpawnItem(LoadedClass); // 이미 있으면 즉시 스폰
        }
        else
        {
            // 없으면 비동기 로드 요청 (님이 만든 헬퍼 사용)
            UEPAsyncLoadHelper::RequestAsyncLoad<AActor>(SoftItemClass,
                [this](TSubclassOf<AActor> LoadedClass)
                {
                    if (LoadedClass)
                    {
                        SpawnItem(LoadedClass); // 비동기 로드 후 스폰
                    }
                }
            );
        }
    }
}

void UEPLootComponent::SpawnItem(TSubclassOf<AActor> ItemClassToSpawn)
{
    if (!ItemClassToSpawn || !GetWorld()) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    // 스폰 위치 계산 (위치 보정 + 랜덤 분산)
    FVector SpawnLocation = GetGroundLocation(Owner);
    //SpawnLocation.Z += 50.0f;

    // 아이템들이 한 점에 겹치지 않도록 위치 조정
    // * * * 아이템이 하나일 경우 -> 고정 위치 * * * 
    float RandomRange = 40.0f;
    SpawnLocation.X += FMath::FRandRange(-RandomRange, RandomRange);
    SpawnLocation.Y += FMath::FRandRange(-RandomRange, RandomRange);

    // ==============================================================================================================
    // 스폰 회전 계산 (랜덤 회전)
    // 아이템이 떨어질 때 제각각 다른 방향을 보고 있도록 함
    FRotator SpawnRotation = FRotator::ZeroRotator;
    SpawnRotation.Yaw = FMath::FRandRange(0.0f, 360.0f);

    // 스폰 파라미터 설정
    FActorSpawnParameters SpawnParams;
    // 위치가 조금 겹치더라도 강제로 스폰 (아이템끼리 밀어내도록)
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = Owner; // (선택) 누가 떨궜는지 기록

    // ==============================================================================================================
    
    // item drop 하는 공용 라이브러리(내부 헬퍼 함수) 호출
    AEPDroppedItem* DroppedActor = UEPItemLibrary::SpawnDroppedItem(
        this,
        DroppedItemClass, // 컴포넌트가 알고 있는 껍데기 클래스
        ItemClassToSpawn, // 내용물
        SpawnLocation
    );
    // ----------------------------------------------------------------

    if (DroppedActor)
    {
        // Spawn Item 데이터 초기화
        //DroppedActor->InitializeDrop(ItemClassToSpawn, 1);

        // ============== 물리 힘 가하기 ==============
        // 아이템의 RootComponent(보통 StaticMesh)를 가져옴
        UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(DroppedActor->GetRootComponent());

        // 물리 시뮬레이션이 켜져 있는 아이템이라면 힘을 가함
        if (RootPrim && RootPrim->IsSimulatingPhysics())
        {
            // 힘의 방향: 위쪽(Z) + 랜덤한 옆 방향
            FVector ImpulseDirection = FVector(
                FMath::FRandRange(-0.5f, 0.5f), // X축 랜덤
                FMath::FRandRange(-0.5f, 0.5f), // Y축 랜덤
                1.0f // Z축 (위로)
            );

            // 벡터 정규화 (방향만 남김)
            ImpulseDirection.Normalize();

            // 힘의 세기 (조절 필요)
            float ImpulseStrength = FMath::RandRange(200.0f, 400.0f);

            // 충격(Impulse) 적용
            RootPrim->AddImpulse(ImpulseDirection * ImpulseStrength, NAME_None, true);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DroppedActor is null"));
    }

}

// Called every frame
void UEPLootComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

// 바닥 찾아서 위치(FVector) 반환 함수
FVector UEPLootComponent::GetGroundLocation(AActor* OwnerActor, float TraceDist, float OffsetZ) const
{
    if (!OwnerActor || !GetWorld()) return FVector::ZeroVector;

    FVector StartLocation = OwnerActor->GetActorLocation();
    FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, TraceDist);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerActor); // 자기 자신 무시

    // 레이캐스트 발사 (WorldStatic 레이어 감지)
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_WorldStatic,
        Params
    );

    // 바닥을 찾은 경우
    if (bHit)
    {
        // 바닥 위치 + 오프셋(살짝 띄우기) 반환
        return HitResult.Location + FVector(0.0f, 0.0f, OffsetZ);
    }

    //바닥을 못 찾은 경우(낭떠러지 등) -> 중앙 반환
    // 캐릭터가 아니라면 그냥 현재 위치 반환 (최후의 수단)
    return StartLocation;
}

