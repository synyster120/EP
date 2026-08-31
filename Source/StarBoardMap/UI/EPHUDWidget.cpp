
#include "UI/EPHUDWidget.h"
#include "Components/Image.h"
#include "UI/EPHealthUnitWidget.h"
#include "Components/EPStatComponent.h"
#include "Data/EPCharacterTypes.h"


void UEPHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (APlayerController* PlayerController = GetOwningPlayer()) // UI(자신)을 띄운 플레이어 컨트롤러
    {
        if (APawn* CurrentPawn = PlayerController->GetPawn())
        {
            // 빙의가 이미 끝났다면 바로 바인딩
            TryBindToStatComponent(CurrentPawn);
        }
        else
        {
            // 빙의 전 이라면 예약
            PlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &UEPHUDWidget::HandlePawnPossessed);
        }
    }
}

void UEPHUDWidget::HandlePawnPossessed(APawn* OldPawn, APawn* NewPawn)
{
    if (NewPawn)
    {
        // StatComponent에 바인딩
        TryBindToStatComponent(NewPawn);
    }
}

void UEPHUDWidget::TryBindToStatComponent(APawn* InPawn)
{
    if (!InPawn) return;

    // pawn의 StatComponent를 찾아 바인딩
    if (UEPStatComponent* StatComp = InPawn->FindComponentByClass<UEPStatComponent>())
    {
        StatComp->OnHealthChanged_Two.AddUniqueDynamic(this, &UEPHUDWidget::UpdateHealthFloat);

        // 체력 초기화
        FEPHealthInfo CurrentStat = StatComp->GetHealthInfo();
        if (CurrentStat.HealthType == EEPHealthType::HealthBlock)
        {
            UpdateHealthFloat_Implementation(CurrentStat.CurrentBlocks, CurrentStat.MaxBlocks);
        }
    }
}

void UEPHUDWidget::UpdateHealthFloat_Implementation(float NewHealth, float MaxHealth)
{
    if (!HealthContainer || !HealthUnitClass) return;

    // 실수형 데이터를 정수형으로 변환  (소수점 내림 사용) CeilToInt(반올림) 
    const int32 CurrentHealthInt = FMath::FloorToInt(NewHealth);
    const int32 MaxHealthInt = FMath::FloorToInt(MaxHealth);

    // 현재 컨테이너에 있는 자식 위젯의 개수 확인
    const int32 CurrentChildCount = HealthContainer->GetChildrenCount();

    // 개수 동기화 (MaxHealth가 변했을 때만 실행됨)
    if (CurrentChildCount != MaxHealthInt)
    {
        if (CurrentChildCount < MaxHealthInt)
        {
            // 부족하면 더 생성
            const int32 Needed = MaxHealthInt - CurrentChildCount;
            for (int32 i = 0; i < Needed; ++i)
            {
                UUserWidget* NewUnit = CreateWidget<UUserWidget>(this, HealthUnitClass);
                if (NewUnit)
                {
                    HealthContainer->AddChild(NewUnit);
                }
            }
        }
        else
        {
            // 많으면 뒤에서부터 제거 (RemoveChildAt은 5.4 등 최신 버전 패널 지원 여부 확인 필요, 안되면 Loop로 제거)
            // 안전하게 뒤에서부터 제거하는 방식
            while (HealthContainer->GetChildrenCount() > MaxHealthInt)
            {
                HealthContainer->RemoveChildAt(HealthContainer->GetChildrenCount() - 1);
            }
        }
    }

    // 상태 업데이트 (체력 있는 상태/ 체력 없는 상태)
    for (int32 i = 0; i < HealthContainer->GetChildrenCount(); ++i)
    {
        // 자식 위젯 가져오기
        UWidget* ChildWidget = HealthContainer->GetChildAt(i);
        UEPHealthUnitWidget* HealthUnit = Cast<UEPHealthUnitWidget>(ChildWidget); // 형변환
        if (HealthUnit)
        {
            // i가 현재 체력보다 작으면 꽉 찬 상태(true), 아니면 빈 상태(false)
            bool bFull = (i < CurrentHealthInt);

            // 상태 변경 함수 호출 (BP 내부에서 이미지 변경)
            HealthUnit->UpdateState(bFull);

        }
    }
}
