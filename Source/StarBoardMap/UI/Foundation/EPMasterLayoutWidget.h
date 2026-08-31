#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EPMasterLayoutWidget.generated.h"

class UCommonActivatableWidgetStack;
class UCommonActivatableWidget;
class UImage;

/**
 *  플레이어가 소유한 가장 최상위 HUD 위젯
 */

UCLASS()
class STARBOARDMAP_API UEPMasterLayoutWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    // 블루프린트에서 바인딩할 Stack (메뉴들이 쌓이는 곳)
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetStack> CommonMenuStack;

    // 블루프린트에서 바인딩할 Stack (모달들이 쌓이는 곳)
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetStack> CommonModalStack;

    // 블루프린트에서 바인딩할 배경 (Stack보다 뒤에 렌더링되게 배치)
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> BackdropImage;

    bool bIsBackdropVisible = false;

private:
    // 스택 내용물이 바뀔 때마다 호출될 함수
    void HandleMenuStackChanged(UCommonActivatableWidget* DisplayedWidget);
};
