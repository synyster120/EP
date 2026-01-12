
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "Data/EPCharacterTypes.h"
#include "EPCharacterBase.generated.h"

// 전방 선언
class UAnimMontage;
class UEPCharacterAnimationData;
class UEPMovementLockComponent;

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDataInitializedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionEnded, FGameplayTag, ActionTag);

UCLASS()
class STARBOARDMAP_API AEPCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	FORCEINLINE UEPMovementLockComponent* GetMovementLockComponent() const { return MovementLockComponent; }
	//UPROPERTY(BlueprintAssignable, Category = "Events")
	//FOnMontageEnded tempdelegate;
	AEPCharacterBase();
	//* @param OnMontageEndedDelegate 몽타주 종료/중단 시 호출될 델리게이트
	void PlayAnimationByTag(FGameplayTag NewTag, const FOnMontageEnded& OnMontageEndedDelegate);

	// 현재 상태 Getter/Setter
	inline EEPCharacterState GetCurrentState() const { return CurrentState; }
	inline void SetCurrentState(EEPCharacterState NewState) { CurrentState = NewState; };
	FORCEINLINE TObjectPtr<UEPCharacterAnimationData> GetAnimDataAsset() { return AnimDataAsset; };

// ---- 몽타주 재생 방법 변경 ------
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnActionEnded OnActionEnded;

	void PlayAnimationByTag(FGameplayTag NewTag);

// ----------------------------------<

protected:
	virtual void BeginPlay() override;

	// 이름, 외형 등의 초기화
	virtual void InitializeCharacterData();

// ---- 몽타주 재생 방법 변경 ------
private:
	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	FGameplayTag CurrentActionTag; // 현재 재생 중인 태그 저장

// ----------------------------------<

public:
	FOnCharacterDataInitializedDelegate OnDataInitialized;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UEPCharacterAnimationData> AnimDataAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEPMovementLockComponent> MovementLockComponent;

	// 캐릭터 현재 행동 상태
	EEPCharacterState CurrentState = EEPCharacterState::Idle;

};
