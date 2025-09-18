
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

};
