
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "Data/EPCharacterTypes.h"
#include "EPCharacterBase.generated.h"

// 전방 선언
class UAnimMontage;
class UEPCharacterAnimationData;

UCLASS()
class STARBOARDMAP_API AEPCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEPCharacterBase();
	void PlayAnimationByTag(FGameplayTag NewTag);

	// 현재 상태 Getter/Setter
	inline EEPCharacterState GetCurrentState() const { return CurrentState; }
	inline void SetCurrentState(EEPCharacterState NewState) { CurrentState = NewState; };

protected:
	virtual void BeginPlay() override;

	// 이름, 외형 등의 초기화
	virtual void InitializeCharacterData();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UEPCharacterAnimationData> AnimDataAsset;

	// 캐릭터 현재 행동 상태
	EEPCharacterState CurrentState = EEPCharacterState::Idle;

};
