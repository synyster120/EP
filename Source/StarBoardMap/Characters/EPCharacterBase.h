
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EPCharacterBase.generated.h"

// 전방 선언
class UAnimMontage;

UCLASS()
class STARBOARDMAP_API AEPCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEPCharacterBase();

protected:
	virtual void BeginPlay() override;

	// 이름, 외형 등의 초기화
	virtual void InitializeCharacterData();


//	/** 캐릭터의 애니메이션 데이터 애셋 */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
//	TObjectPtr<UEPCharacterAnimationData> AnimDataAsset;
};
