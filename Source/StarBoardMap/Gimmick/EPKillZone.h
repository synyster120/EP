
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EPKillZone.generated.h"

class UBoxComponent;

UCLASS()
class STARBOARDMAP_API AEPKillZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AEPKillZone();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KillZone")
	UBoxComponent* CollisionBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
