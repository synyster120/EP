// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interfaces/EPCombatQueryInterface.h"
#include "EPCannon.generated.h"

class UEPSkillComponent;
class UEPSkillDataAsset;

UCLASS()
class STARBOARDMAP_API AEPCannon : public AActor, public IEPCombatQueryInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEPCannon();

	UPROPERTY(EditAnywhere, Category = "Skill | Rate")
	float AttackRate = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Skill | Looping")
	bool AttackLooping = true;

	// 에디터 뷰포트에서 직접 드래그 가능한 3D 위젯 - 발사체 공격의 중심 되는 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (MakeEditWidget = true), Category = "Combat")
	FVector GuardOffset;

	// interface
	virtual FVector GetMuzzleLocation() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEPSkillComponent> SkillComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TArray<TSoftObjectPtr<UEPSkillDataAsset>> DefaultSkills;

	UPROPERTY()
	FTimerHandle AttackHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
