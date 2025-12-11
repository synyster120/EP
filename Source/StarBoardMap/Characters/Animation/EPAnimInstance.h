// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Data/EPItemTypes.h"
#include "EPAnimInstance.generated.h"

/**
 * 
 */

 // 아이템 애니메이션 타입
//enum class EEPItemAnimType : uint8
//{
//	None        UMETA(DisplayName = "맨손"),
//	Hammer      UMETA(DisplayName = "망치"),
//	Sword       UMETA(DisplayName = "한손검"),
//	Potion      UMETA(DisplayName = "포션 마시기"),
//	Gun         UMETA(DisplayName = "총")
//};

UCLASS()
class STARBOARDMAP_API UEPAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool IsGotHammer_C = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool IsGotPotion_C = false;

	// 리플렉션 없이 접근할 수 있는 변수
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	EEPItemAnimType CurrentWeaponType = EEPItemAnimType::None;*/

};
