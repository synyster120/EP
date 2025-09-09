// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterAnimationData.generated.h"

// 전방 선언
class UAnimMontage;

/**
 *	피격 타입따른 애니메이션, 죽음 애니메이션	- 데이터 에셋
 */

 // 피격 반응(강도) 타입
UENUM(BlueprintType)
enum class EHitReactionType : uint8
{
    Light       UMETA(DisplayName = "가벼운 피격"), // 가벼운 피격
    Heavy       UMETA(DisplayName = "강한 피격"), // 강한 피격
    Knockdown   UMETA(DisplayName = "넘어짐"), // 넘어짐
    Launch      UMETA(DisplayName = "공중으로 뜸") // 공중으로 뜸
};

// 캐릭터 애니메이션 데이터를 정의하는 데이터 에셋
UCLASS(BlueprintType)
class STARBOARDMAP_API UCharacterAnimationData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 피격 반응: 피격 타입에 따라 재생할 애니메이션 몽타주를 매핑합니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	TMap<EHitReactionType, TObjectPtr<UAnimMontage>> HitReactionMontages;

	// 죽음 애니메이션: 캐릭터가 죽을 때 재생할 애니메이션 몽타주입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	TSoftObjectPtr<UAnimMontage> DeathAnimationMontage;
};
