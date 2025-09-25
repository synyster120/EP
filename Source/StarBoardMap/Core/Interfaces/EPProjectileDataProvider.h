
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/EPSkillTypes.h"
#include "EPProjectileDataProvider.generated.h"


UINTERFACE(MinimalAPI)
class UEPProjectileDataProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STARBOARDMAP_API IEPProjectileDataProvider
{
	GENERATED_BODY()

public:
	// 투사체 데이터를 반환하는 함수 규칙
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	const bool GetProjectileData(int32 PhaseIndex, FEPProjectileData& OutProjectileData) const;
};
