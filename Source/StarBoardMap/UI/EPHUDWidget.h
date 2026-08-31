#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/PanelWidget.h" // 컨테이너 부모 클래스
#include "EPHUDWidget.generated.h"

class UImage;

/**
 * 
 */

UCLASS()
class STARBOARDMAP_API UEPHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "HUD")
    void UpdateHealthFloat(float NewHealth, float MaxHealth);

    virtual void NativeConstruct() override;

protected:
    // UI 에디터에서 배치할 컨테이너 (HorizontalBox나 WrapBox 추천)
    UPROPERTY(meta = (BindWidget)) // BindWidget : 블루프린트의 위젯과 자동 연결
    TObjectPtr<UPanelWidget> HealthContainer;

    // 생성할 체력 칸 위젯 클래스 (BP에서 설정)
    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    TSubclassOf<UUserWidget> HealthUnitClass;
    
private:
    // 컨트롤러가 새로운 폰에 빙의했을 때 호출될 이벤트
    UFUNCTION()
    void HandlePawnPossessed(APawn* OldPawn, APawn* NewPawn);

    // 실제 델리게이트 바인딩을 수행할 함수
    void TryBindToStatComponent(APawn* InPawn);

};
