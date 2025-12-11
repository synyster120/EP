
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EPLootListenerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UEPLootListenerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *		역할 : 아이템이 사라질 때 듣는 귀 (ex. 솥에서 아이템이 사라질 때 신호 들음)
 */

class STARBOARDMAP_API IEPLootListenerInterface
{
	GENERATED_BODY()

public:
	// "아이템이 삭제될 테니 목록에서 빼라"는 신호
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Loot")
	void OnItemRemoved(AActor* RemovedItem);

};
