
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EPCheckpoint.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()
class STARBOARDMAP_API AEPCheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AEPCheckpoint();

protected:
	virtual void BeginPlay() override;

    // 플레이어가 닿았는지 판정할 콜리전
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* CollisionBox;

    // 리스폰 시 플레이어가 바라볼 방향을 시각적으로 나타낼 화살표
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UArrowComponent* ArrowDirection;

    // 오버랩 이벤트 처리를 위한 함수
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
