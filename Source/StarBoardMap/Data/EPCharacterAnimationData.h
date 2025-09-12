// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EPCharacterAnimationData.generated.h"

// 전방 선언
class UAnimMontage;

/**
 *	피격 타입따른 애니메이션, 죽음 애니메이션	- 데이터 에셋
 */

 // 피격 반응(강도) 타입
UENUM(BlueprintType)
enum class EEPHitReactionType : uint8
{
    Light       UMETA(DisplayName = "가벼운 피격"), // 가벼운 피격
    Heavy       UMETA(DisplayName = "강한 피격"), // 강한 피격
    Knockdown   UMETA(DisplayName = "넘어짐"), // 넘어짐
    Launch      UMETA(DisplayName = "공중으로 뜸") // 공중으로 뜸
};

// 캐릭터 애니메이션 데이터를 정의하는 데이터 에셋
UCLASS(BlueprintType)
class STARBOARDMAP_API UEPCharacterAnimationData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 피격 반응: 피격 타입에 따라 재생할 애니메이션 몽타주를 매핑
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	TMap<EEPHitReactionType, TSoftObjectPtr<UAnimMontage>> HitReactionMontages;

	// 죽음 애니메이션: 캐릭터가 죽을 때 재생할 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	TSoftObjectPtr<UAnimMontage> DeathAnimationMontage;

	// 스킬 시전 애니메이션: Tag에 맞는 스킬을 시전할 때 재생할 애니메이션 몽타주 (SkillPhases와 연결)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	TMap<FGameplayTag, TSoftObjectPtr<UAnimMontage>> SkillAnimationMontage;

	// 줍기, 문열기 등 동작 시 재생할 애니메이션 몽타주 (일회성 행동)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	TMap<FGameplayTag, TSoftObjectPtr<UAnimMontage>> InteractionMontages;

	// 잡기 포즈 : 무언갈 소유할 때 기본이 될 포즈 (지속 행동 -상하체 등 분리 후 조합됨)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	TMap<FGameplayTag, TSoftObjectPtr<UAnimSequence>> GripPoses;
};
