
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
//#include "Path/To/Your/AItemActor.h"
//#include "Items/EPItemBase.h"
#include "EPItemTypes.generated.h"

// 전방 선언
//class AItemActor;
class UTexture2D;
class AEPItemBase;

/**
 *  아이템 관련 데이터 타입 정의
 */

 // 아이템 타입 정의
UENUM(BlueprintType)
enum class EEPItemType : uint8
{
    Consumable  UMETA(DisplayName = "소모품"),
    Equipment   UMETA(DisplayName = "장비"),
    Quest       UMETA(DisplayName = "퀘스트"),
    Etc         UMETA(DisplayName = "기타")
};

// 아이템 애니메이션 타입
UENUM(BlueprintType)
enum class EEPItemAnimType : uint8
{
	None        UMETA(DisplayName = "맨손"),
	Hammer      UMETA(DisplayName = "망치"),
	Sword       UMETA(DisplayName = "한손검"),
	Potion      UMETA(DisplayName = "포션 마시기"),
    Object      UMETA(DisplayName = "양손 아이템 들기"),
	Gun         UMETA(DisplayName = "총")
};
 
// 아이템 정보 구조체 (데이터 테이블용)
USTRUCT(BlueprintType)
struct FEPItemData : public FTableRowBase
{
    GENERATED_BODY()

    // 아이템 타입
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    EEPItemType ItemType;

    // 아이템 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FText ItemName;

    // 아이템 설명
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FText Description;

    // 아이템 아이콘 이미지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    TSoftObjectPtr<UTexture2D> IconImage;

    // 행동을 정의하는 태그
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FGameplayTag PickupInteractionTag; // 예: Item.Interaction.Pickup.Low, Item.Interaction.Pickup.Heavy

    // 중첩 정보
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    bool bIsStackable = true;

    // 최대 중첩 개수 (bIsStackable=true 일 때만 활성화)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data", meta = (EditCondition = "bIsStackable"))
    int32 MaxStackSize = 99;

    //// 월드 외형
    //UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    //TSoftObjectPtr<UStaticMesh> WorldMesh;
    
    /** 이 아이템 데이터가 월드에 스폰될 때 사용할 액터 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Actor")
    TSoftClassPtr<AEPItemBase> SoftItemClass;

    // 사용 효과 클래스 (버프, 디버프 등)
    /*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    TSubclassOf<UItemEffectBase> ItemEffectClass;*/
};

// 아이템 드랍 시 아이템마다 필요한 설정(개수 등)
USTRUCT(BlueprintType)
struct FEPDropItemInfo
{
    GENERATED_BODY()

public:
    // 드랍할 아이템의 행(데이터 테이블 지정 후 행 지정)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FDataTableRowHandle DropItemRow;

    // 드랍 확률
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float DropChance; 

    // 최소 드랍 개수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 MinQuantity; 

    // 최대 드랍 개수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 MaxQuantity;
};

// 드랍 아이템 테이블
USTRUCT(BlueprintType)
struct FEPDropTable : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TArray<FEPDropItemInfo> DropItems;

};