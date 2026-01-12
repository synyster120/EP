// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Data/EPCombatTypes.h"
#include "EPCharacterAnimationData.generated.h"

// 전방 선언
class UAnimMontage;

/**
 *	피격 타입따른 애니메이션, 죽음 애니메이션	- 데이터 에셋
 */

USTRUCT(BlueprintType)
struct FEPHitTagThreshold
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float DamageThreshold; // 이 수치 이상일 때

	UPROPERTY(EditAnywhere)
	FGameplayTag HitTag;   // 이 태그를 사용
};

USTRUCT(BlueprintType)
struct FEPActionAnimationInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> Montage;

	// 이 애니메이션이 실행될 때 변경될 캐릭터의 상태
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEPCombatMontageType TargetState;
};

// 캐릭터 애니메이션 데이터를 정의하는 데이터 에셋
UCLASS(BlueprintType)
class STARBOARDMAP_API UEPCharacterAnimationData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 몽타주 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Data")
	TMap<FGameplayTag, FEPActionAnimationInfo> ActionMap;

	// 피격 강도 정의
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Data ㅣ Hit")
	TArray<FEPHitTagThreshold> HitType;

	// 죽음 애니메이션: 캐릭터가 죽을 때 재생할 애니메이션 데이터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data ㅣ Die", meta = (ToolTip = "State.Dead 태그와 매칭되는 몽타주"))
	FEPActionAnimationInfo DeadAnimationInfo;

	// 기본 애니메이션 : 몽타주를 찾지 못했을 때 기본적으로 재생할 애니메이션 데이터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data ㅣ Default", meta = (ToolTip = "Action.Hit.Default 태그와 매칭되는 몽타주"))
	FEPActionAnimationInfo DefaultHitInfo;

};
