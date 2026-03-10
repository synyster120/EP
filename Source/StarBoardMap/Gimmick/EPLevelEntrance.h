
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EPLevelEntrance.generated.h"

class UArrowComponent;

// 레벨의 시작지점과 끝지점

UCLASS()
class STARBOARDMAP_API AEPLevelEntrance : public AActor
{
	GENERATED_BODY()
	
public:	
	AEPLevelEntrance();

protected:
    // 액터의 기본 위치를 잡아줄 씬 컴포넌트 (루트)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* RootSceneComponent;

    // 플레이어가 스폰될 정확한 위치와 바라볼 방향을 시각적으로 표시해줄 화살표
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UArrowComponent* SpawnDirection;

};
