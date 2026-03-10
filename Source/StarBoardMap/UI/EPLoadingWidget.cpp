

#include "UI/EPLoadingWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/Overlay.h"

#include "Components/CanvasPanelSlot.h"

void UEPLoadingWidget::InitializeWidgetState()
{
    // 진행률 0으로 초기화
    if (LoadingBar)
    {
        LoadingBar->SetPercent(0.0f);
        EPTargetPercent = 0.0f;
        EPCurrentPercent = 0.0f;
    }

    if (FadeAnim_Image)
    {
        FadeAnim_Image->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (LoadingUI)
    {
        LoadingUI->SetVisibility(ESlateVisibility::Visible);
    }

    //화면 불투명도 및 가시성 초기화
    SetVisibility(ESlateVisibility::Visible);
    SetRenderOpacity(1.0f);
}

void UEPLoadingWidget::UpdateProgress(float NewPercent)
{
    EPTargetPercent = NewPercent;
}

// fade out
void UEPLoadingWidget::PlayFadeOutAnimation()
{
    if (FadeOut_Anim)
    {
        if (FadeAnim_Image)
        {
            FadeAnim_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        }
        // Forward: 0 -> 1 (투명 -> 검은색) 가정
        PlayAnimation(FadeOut_Anim);
        UE_LOG(LogTemp, Warning, TEXT("[ERROR ANIM] fade out play"));
    }
    else
    {
        // 애니메이션이 없으면 즉시 종료 처리 (예외 처리)
        HandleFadeOutAnimFinished();
    }
}

// fade in
void UEPLoadingWidget::PlayFadeInAnimation()
{
    if (FadeIn_Anim)
    {
        // 모든 애니메이션 종료
        StopAllAnimations();
            
        if (LoadingUI)
        {
            LoadingUI->SetVisibility(ESlateVisibility::Collapsed);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Loading ui - overlay is nullptr"));
        }

        // 가시성을 켜고, 불투명도를 강제로 시작점(1.0)으로 세팅
        if (FadeAnim_Image)
        {
            FadeAnim_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            FadeAnim_Image->SetRenderOpacity(1.0f); // 0.01초 종료 방지
        }

        // Forward: 1 -> 0 (검은색 -> 투명) 가정
        PlayAnimation(FadeIn_Anim);
        UE_LOG(LogTemp, Warning, TEXT("[ERROR ANIM] fade in play"));
    }
    else
    {
        // 애니메이션이 없으면 즉시 종료 처리
        HandleFadeInAnimFinished();
    }
}

void UEPLoadingWidget::NativeConstruct()
{
    Super::NativeConstruct();

    static bool bIsBound = false;
    // 애니메이션 종료 델리게이트 바인딩
    if (!bIsBound)
    {
        if (FadeOut_Anim)
        {
            FWidgetAnimationDynamicEvent OutDelegate;
            OutDelegate.BindDynamic(this, &UEPLoadingWidget::HandleFadeOutAnimFinished);
            BindToAnimationFinished(FadeOut_Anim, OutDelegate);
        }

        if (FadeIn_Anim)
        {
            FWidgetAnimationDynamicEvent InDelegate;
            InDelegate.BindDynamic(this, &UEPLoadingWidget::HandleFadeInAnimFinished);
            BindToAnimationFinished(FadeIn_Anim, InDelegate);
        }
        bIsBound = true;
    }

    if (Anim)
    {
        // 머티리얼 생성 시 멤버 변수에 저장하여 수명 연장
        FlipbookMaterialInstance = Anim->GetDynamicMaterial();

    }
}

void UEPLoadingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 이미 목표치에 도달했다면 연산 중단 (중복 실행 방지)
    if (EPCurrentPercent >= 1.0f) return;

    if (!FMath::IsNearlyEqual(EPCurrentPercent, EPTargetPercent, 0.001f))
    {
        EPCurrentPercent = FMath::FInterpTo(EPCurrentPercent, EPTargetPercent, InDeltaTime, 2.0f);

        if (LoadingBar && StarIcon)
        {
            // 바의 실제 픽셀 너비 구하기
            float BarWidth = LoadingBar->GetCachedGeometry().GetLocalSize().X;

            LoadingBar->SetPercent(EPCurrentPercent);

            // Icon 이동할 위치 계산
            float NewX = BarWidth * EPCurrentPercent;

            // 아이콘 위치 적용
            if (UCanvasPanelSlot* IconSlot = Cast<UCanvasPanelSlot>(StarIcon->Slot))
            {
                // Y축은 기존 값 유지, X축만 변경
                FVector2D CurrentPos = IconSlot->GetPosition();
                IconSlot->SetPosition(FVector2D(NewX, CurrentPos.Y));
            }

            // 혹은 0.99f 이상이면 1.0f로 강제 고정 후 로직 실행 (FInterpTo 오차 감안)
            if (EPCurrentPercent >= 0.999f)
            {
                EPCurrentPercent = 1.0f; // 값 강제 고정
                LoadingBar->SetPercent(1.0f);

                // 3. 타이머 중복 생성 방지를 위한 처리 (위젯이 Tick마다 타이머를 새로 만들지 않도록)
                if (!GetWorld()->GetTimerManager().IsTimerActive(FadeAnimDelayTimer))
                {
                    GetWorld()->GetTimerManager().SetTimer(FadeAnimDelayTimer, this, &UEPLoadingWidget::PlayFadeOutAnimation, 0.5f, false);
                }
            }
        }
    }
}

void UEPLoadingWidget::HandleFadeOutAnimFinished()
{
    // fade out 종료 방송
    if (OnFadeOutComplete.IsBound())
    {
        OnFadeOutComplete.Broadcast();
    }
}

void UEPLoadingWidget::HandleFadeInAnimFinished()
{
    if (FadeAnim_Image)
    {
        FadeAnim_Image->SetVisibility(ESlateVisibility::Collapsed);
    }
    // fade in 종료 방송
    if (OnFadeInComplete.IsBound())
    {
        OnFadeInComplete.Broadcast();
    }
}
