
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "EPDroppedItem.generated.h"

class USphereComponent;
class AEPItemBase;

/*
	++ 드랍되는 아이템 (mesh 와 collision만 소유)
*/

UCLASS()
class STARBOARDMAP_API AEPDroppedItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AEPDroppedItem();

	//  스폰 직후, 데이터(Static mesh)를 주입하는 함수
	void InitializeDrop(TSubclassOf<AActor> ItemClass, int32 Quantity);

	// Character가 주울 때 이 정보를 반환해 줄 함수 (Getter)
	TSubclassOf<AEPItemBase> GetOriginalItemClass() const { return OriginalItemClass; }

	// Destroy() 대신 사용하는 실제 파괴 함수
	void DestroyItem();

protected:
	virtual void BeginPlay() override;

	// 사라질 때 (pickup 될 때) overlap 중인 actor한테 유언(자신) 전달
	void NotifyActorsBeforeDestruction();


	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	// 내가 누구인지 기억 (줍기 위해)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSubclassOf<AEPItemBase> OriginalItemClass;

	// 묶음 개수 정보 (예: 포션 5개 묶음)
	/*UPROPERTY(VisibleAnywhere, Category = "Data")
	int32 ItemQuantity;*/

	// 머리 위에 띄울 UI 컴포넌트 ("E 줍기" 위젯)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* PickupWidgetComp;

	bool bStart = false;

public:	
	virtual void Tick(float DeltaTime) override;

	// UI 컨트롤 - 외부(플레이어)에서 호출할 함수들
	void ShowInteractionUI(); 
	void HideInteractionUI();
};
