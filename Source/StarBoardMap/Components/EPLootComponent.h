
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EPLootComponent.generated.h"

struct FEPDropItemInfo;

/*
	** Loot(전리품) Drop 관리 Component	
*/


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARBOARDMAP_API UEPLootComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEPLootComponent();

	UFUNCTION()
	void SpawnLoot();

protected:
	// Dropt Table Data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	FDataTableRowHandle LootDataTableHandle;

	// 드랍 전용 껍데기 클래스를 에디터에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	TSubclassOf<class AEPDroppedItem> DroppedItemClass;

	virtual void BeginPlay() override;

	void ProcessDropItem(const FEPDropItemInfo& DropItemInfo);

	// 실제 스폰을 담당하는 내부 함수
	void SpawnItem(TSubclassOf<AActor> ItemClassToSpawn);

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/*
	 * 오너의 위치를 기준으로 바닥 위치를 찾음
	 * @param OwnerActor : 기준이 될 액터
	 * @param TraceDist : 아래로 검사할 거리 (기본 500)
	 * @param OffsetZ : 바닥에서 띄울 높이 (기본 20)ㅇ
	 * @return 바닥 위치 (찾지 못하면 발밑 근사치 반환)
	 */
	FVector GetGroundLocation(AActor* OwnerActor, float TraceDist = 500.0f, float OffsetZ = 20.0f) const;
		
};
