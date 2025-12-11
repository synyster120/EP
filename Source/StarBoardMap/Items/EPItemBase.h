
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h" // 데이터 테이블 사용 시 필요
#include "Data/EPItemTypes.h"
#include "GameplayTagContainer.h"
#include "EPItemBase.generated.h"


class UTexture2D;

UCLASS()
class STARBOARDMAP_API AEPItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AEPItemBase();

    // [프리뷰] 에디터 전용 컴포넌트 선언
#if WITH_EDITORONLY_DATA
    UPROPERTY(VisibleDefaultsOnly, Category = "Visual")
    UStaticMeshComponent* PreviewMeshComp;
#endif

    // =================================================================
    // 기본 정보 (Identity)
    // =================================================================
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Identity")
    FName ItemID; // 데이터 테이블 RowName과 매칭할 ID (예: "Weapon_001") =============== 이건 Data Table에서만 정의하면 될듯

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Identity")
    FText DisplayName; // UI에 표시될 이름 (예: "녹슨 철검")

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Identity", meta = (MultiLine = true))
    FText Description; // 아이템 설명

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Identity")
    EEPItemType ItemType; // 아이템 타입

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Identity")
    EEPItemAnimType ItemAnimtionType = EEPItemAnimType::None;


    // =================================================================
    // 시각적 정보 (Visuals)
    // =================================================================
    // [UI] 인벤토리 창에 보여줄 아이콘
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Visual")
    UTexture2D* Icon;

    // [World] 바닥에 떨어졌을 때 보여질 메쉬 (DroppedItem이 가져다 씀) - 기본
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Visual")
    UStaticMesh* PickupMesh;

    // [Equip] 캐릭터 손에 장착했을 때 보여질 메쉬 (Character가 가져다 씀)
    // ※ PickupMesh와 EquippedMesh가 다를 수 있음 (예: 접힌 지도 vs 펼친 지도)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Visual")
    UStaticMesh* EquippedMesh;

    // [UI] 아이템 위에 보여줄 위젯 위치 (MakeEditWidget : 레벨에 itembase 배치하면 다이아몬드 형식의 3D핸들 보여줌)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Visual", meta = (MakeEditWidget = true))
    FVector WidgetOffset;


    // =================================================================
    // 수치 정보 (Stats)
    // =================================================================
    // 최대 중첩 개수 (장비는 1, 포션은 99)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Stats")
    int32 MaxStackSize = 1;

    // 장착 시 적용할 오프셋 (위치, 회전, 스케일)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual | Equip")
    FTransform EquippedOffset;

    // 기본 크기
    UPROPERTY(EditDefaultsOnly, Category = "Item Data | Physics")
    FVector ItemScale = FVector(1.0f); 

    // 무게 (kg)
    UPROPERTY(EditDefaultsOnly, Category = "Item Data | Physics")
    float ItemMass = 10.0f; 


    // (선택) 만약 무기라면 데미지
    /*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Stats", meta = (EditCondition = "ItemType == EItemType::Weapon"))
    float AttackDamage;*/

    // (선택) 만약 포션이라면 회복량
    /*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Stats", meta = (EditCondition = "ItemType == EItemType::Consumable"))
    float HealAmount;*/

    // =================================================================
    // 기능 정보 (Functionality)
    // =================================================================
    // 이 아이템을 사용할 때 실행될 몽타주 (예: 마시는 동작, 칼 휘두르는 동작 아님)
    // 공격 모션은 보통 콤보 시스템 등 별도로 관리하지만, 
    // 포션 마시기 같은 '사용(Use)' 동작은 여기에 넣기도 함
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Tag")
    FGameplayTag PickupInteractionTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Tag")
    FGameplayTag DropInteractionTag;


protected:
	virtual void BeginPlay() override;

    virtual void OnConstruction(const FTransform& Transform) override; // 프리뷰용

public:	
	virtual void Tick(float DeltaTime) override;
    FORCEINLINE FGameplayTag GetPickupInteractionTag() { return PickupInteractionTag; };
    FORCEINLINE FGameplayTag GetDropInteractionTag() { return DropInteractionTag; };
};
