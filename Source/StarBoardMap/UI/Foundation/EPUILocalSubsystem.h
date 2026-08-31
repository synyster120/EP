#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "GameplayTagContainer.h"
#include "EPUILocalSubsystem.generated.h"

class UCommonActivatableWidgetStack;
class UCommonActivatableWidget;

/**
 *  요청을 받아 MasterLayout에 창을 띄우는 Local Player Subsystem
 */

 // 스택의 종류를 정의하는 열거형 (확장성 확보)
UENUM(BlueprintType)
enum class EEPUILayer : uint8
{
	Menu,
	Modal
};

// 데이터 테이블 구조체 (Struct)
USTRUCT(BlueprintType)
struct FEPMenuWidgetData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UCommonActivatableWidget> WidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    EEPUILayer TargetLayer;
};

UCLASS()
class STARBOARDMAP_API UEPUILocalSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:
    // UI가 태어날 때 자신의 스택들을 서브시스템에 등록하는 함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    void RegisterMasterLayoutStacks(UCommonActivatableWidgetStack* InMenuStack, UCommonActivatableWidgetStack* InModalStack);

    // Tag 없이 위젯을 띄울 수 있게 해주는 함수 (반환값을 넘겨주어 띄운 직후 델리게이트 바인딩 등을 할 수 있게 함)
    UFUNCTION(BlueprintCallable, Category = "UI")
    UCommonActivatableWidget* PushWidgetToLayer(TSubclassOf<UCommonActivatableWidget> WidgetClass, EEPUILayer Layer);

    // Tag로 Menu 창 띄움
    UFUNCTION(BlueprintCallable, Category = "UI")
    void OpenMenuByTag(FGameplayTag MenuTag);

    void HandleMenuStackCleared(UCommonActivatableWidget* DisplayedWidget);

private:
    TWeakObjectPtr<UCommonActivatableWidgetStack> MenuStack;
    TWeakObjectPtr<UCommonActivatableWidgetStack> ModalStack;

    // 하드코딩
    TSoftObjectPtr<UDataTable> MenuDataTableSoftPtr = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TEXT("/Script/Engine.DataTable'/Game/AssetDynamic/UI/Foundation/DT_MenuWidgetData.DT_MenuWidgetData'")));

    // 로드한 데이터 테이블 캐싱을 위한 포인터
    UPROPERTY()
    UDataTable* CachedMenuDataTable = nullptr;

    bool bReadyData = false;
};
