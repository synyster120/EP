
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/EPCharacterAnimationData.h"
#include "EPCombatQueryInterface.generated.h"

/*
*		플루프린트에서 요청(Query)만 하는 Combat Interface
*/

UINTERFACE(MinimalAPI, NotBlueprintable) // NotBlueprintable로 명시
class UEPCombatQueryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STARBOARDMAP_API IEPCombatQueryInterface
{
	GENERATED_BODY()

public:
	// 피격 반응 애니메이션을 반환한다는 계약
	UFUNCTION( BlueprintCallable, Category = "Combat|Query")
	virtual UAnimMontage* GetHitReactionMontage(EEPHitReactionType HitReactionType) = 0; // 0 : 반드시 재정의
};
