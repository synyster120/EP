
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EPLoadingWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeOutComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeInComplete);

UCLASS()
class STARBOARDMAP_API UEPLoadingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 초기화 및 상태 리셋 함수
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void InitializeWidgetState();

	// 진행률 업데이트 함수
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void UpdateProgress(float NewPercent);

	// Fade Out 애니메이션 재생 함수
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void PlayFadeOutAnimation();

	// Fade In 애니메이션 재생 함수
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void PlayFadeInAnimation();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 애니메이션 종료 시 호출될 내부 함수
	UFUNCTION()
	void HandleFadeOutAnimFinished();
	UFUNCTION()
	void HandleFadeInAnimFinished();
	
public:
	// Subsystem이 구독할 델리게이트 (Fade Out 끝났음 알림)
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnFadeOutComplete OnFadeOutComplete;

	// Subsystem이 구독할 델리게이트 (Fade In 끝났음 알림)
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnFadeInComplete OnFadeInComplete;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> LoadingBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> FadeAnim_Image;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> StarIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Anim;

	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> FlipbookMaterialInstance;
	
	// 애니메이션
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOut_Anim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeIn_Anim;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UOverlay> LoadingUI;

	float EPCurrentPercent;
	float EPTargetPercent; 

	// percent 1.0f next timer
	FTimerHandle FadeAnimDelayTimer;
};
