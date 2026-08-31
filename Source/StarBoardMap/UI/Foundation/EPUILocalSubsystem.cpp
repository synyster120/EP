
#include "UI/Foundation/EPUILocalSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"
#include "CommonInputSubsystem.h"


void UEPUILocalSubsystem::RegisterMasterLayoutStacks(UCommonActivatableWidgetStack* InMenuStack, UCommonActivatableWidgetStack* InModalStack)
{
    // MasterLayout이 생성될 때 넘겨준 스택 포인터들을 약한 참조로 저장
    MenuStack = InMenuStack;
    ModalStack = InModalStack;

    bReadyData = true;

    if (InMenuStack)
    {
        // '위젯 변동 시 방송'에 바인딩
        InMenuStack->OnDisplayedWidgetChanged().AddUObject(this, &UEPUILocalSubsystem::HandleMenuStackCleared);
    }
}

UCommonActivatableWidget* UEPUILocalSubsystem::PushWidgetToLayer(TSubclassOf<UCommonActivatableWidget> WidgetClass, EEPUILayer Layer)
{
    if (!WidgetClass || !bReadyData) return nullptr;
    UE_LOG(LogTemp, Warning, TEXT("push widget- Layer"));

    UCommonActivatableWidgetStack* TargetStack = nullptr;

    // Enum값에 따라 목적지 스택 결정
    switch (Layer)
    {
    case EEPUILayer::Menu:  TargetStack = MenuStack.Get(); break;
    case EEPUILayer::Modal: TargetStack = ModalStack.Get(); break;
    }

    // 약한 참조가 아직 살아있는지(UI가 파괴되지 않았는지) 검사
    if (TargetStack)
    {
        UCommonActivatableWidget* ActiveWidget = TargetStack->GetActiveWidget();
        // 중복 창 생성 방어 로직
        if (ActiveWidget && ActiveWidget->GetClass() == WidgetClass)
        {
            // 이미 화면 맨 앞에 떠 있으므로 생성 무시
            return nullptr;
        }

        // Common UI 스택에 위젯을 밀어넣고, 생성된 위젯의 포인터를 반환
        return TargetStack->AddWidget<UCommonActivatableWidget>(WidgetClass);
    }

    UE_LOG(LogTemp, Warning, TEXT("EPUISubsystem: Target Stack is invalid or destroyed!"));
    return nullptr;
}

void UEPUILocalSubsystem::OpenMenuByTag(FGameplayTag MenuTag)
{
    // 동기 로딩 & 캐싱 
    if (!CachedMenuDataTable)
    {
        // 지정된 소프트 경로의 에셋을 메모리에 즉시 퍼 올립니다.
        CachedMenuDataTable = MenuDataTableSoftPtr.LoadSynchronous();
    }

    if (!CachedMenuDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("OpenMenuByTag: Failed Menu UI DataTable load"));
        return;
    }

    // data table 사용하여 Tag 탐색
    FEPMenuWidgetData* RowData = CachedMenuDataTable->FindRow<FEPMenuWidgetData>(MenuTag.GetTagName(), TEXT("FindMenuWidget"));

    if (RowData && RowData->WidgetClass)
    {
        // 화면에 띄움
        PushWidgetToLayer(RowData->WidgetClass, RowData->TargetLayer);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OpenMenuByTag: There is no corresponding tag (%s) data on the table!"), *MenuTag.ToString());
    }

}

// 스택이 비었을 때, 입력 포커스 되돌리기 (오류 방지)
void UEPUILocalSubsystem::HandleMenuStackCleared(UCommonActivatableWidget* DisplayedWidget)
{
    if (DisplayedWidget == nullptr) // 스택이 빈 상태
    {
        // 입력 모드를 게임 플레이 전용으로 강제 초기화
        APlayerController* PlayerController = GetLocalPlayer()->GetPlayerController(GetWorld());
        if (PlayerController)
        {
            FInputModeGameOnly InputMode;
            PlayerController->SetInputMode(InputMode);
            PlayerController->bShowMouseCursor = false; // 마우스 커서 숨기기

            // Common UI 내부의 입력 상태도 초기화
            ULocalPlayer* LocalPlayer = GetLocalPlayer();
            if (LocalPlayer)
            {
                if (UCommonInputSubsystem* CommonInputSys = LocalPlayer->GetSubsystem<UCommonInputSubsystem>())
                {
                    // 입력 장치를 키보드/패드 모드로 강제 동기화
                    CommonInputSys->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, FName("Gameplay"), false);
                }
            }
        }
    }
}
