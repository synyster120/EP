
#include "UI/Foundation/EPMasterLayoutWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Components/Image.h"
#include "UI/Foundation/EPUILocalSubsystem.h"

void UEPMasterLayoutWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (BackdropImage)
    {
        BackdropImage->SetVisibility(ESlateVisibility::Collapsed);
    }

    // MenuStack의 상태 변화 바인딩
    if (CommonMenuStack)
    {
        CommonMenuStack->OnDisplayedWidgetChanged().AddUObject(this, &ThisClass::HandleMenuStackChanged);
    }

    // Stack 설정 (subsystem이 widget을 push하기 위함)
    if (ULocalPlayer* LocalPlayer = GetOwningLocalPlayer())
    {
        UEPUILocalSubsystem* UISubsystem = LocalPlayer->GetSubsystem<UEPUILocalSubsystem>();
        if (UISubsystem)
        {
            // Stack 초기화
            UISubsystem->RegisterMasterLayoutStacks(CommonMenuStack, CommonModalStack);
        }
    }
}

// menu stack이 0 또는 1이상 일 경우, backdrop img 비활성화 및 활성화
void UEPMasterLayoutWidget::HandleMenuStackChanged(UCommonActivatableWidget* DisplayedWidget)
{
    const bool bHasWidgets = CommonMenuStack->GetNumWidgets() > 0; // 스택 확인

    if (!BackdropImage || !CommonMenuStack) return;

    if (bHasWidgets && !bIsBackdropVisible ) // 스택 있고 backdrop img 숨겨진 상태
    {
        // backdrop image 활성화
        bIsBackdropVisible = true;
        BackdropImage->SetVisibility(ESlateVisibility::Visible);
    }
    else if (!bHasWidgets && bIsBackdropVisible) // 스택 없는데 backdrop img 켜진 상태
    {
        // backdrop image 비활성화
        bIsBackdropVisible = false;
        BackdropImage->SetVisibility(ESlateVisibility::Collapsed);
    }
}
