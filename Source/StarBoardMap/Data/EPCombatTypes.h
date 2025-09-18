// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EPCombatTypes.generated.h"

/**
 *     공격과 관련된 타입 모음
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


/**
 * 데미지 이벤트에 대한 모든 정보를 담는 구조체
 * '사건 파일'처럼 데미지에 대한 상세한 정보를 전달하는 역할
 */
USTRUCT(BlueprintType)
struct FEPDamageInfo
{
    GENERATED_BODY()

public:
    /** 기본 데미지량 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float BaseDamage = 0.0f;

    /** 공격을 최종적으로 지시한 컨트롤러 (가해자) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    TWeakObjectPtr<AController> InstigatorController;

    /** 데미지를 물리적으로 유발한 액터 (무기, 발사체 등) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    TWeakObjectPtr<AActor> DamageCauser;

    /** 피해가 발생한 위치 (헤드샷 등 부위 판정용) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    FVector_NetQuantize HitLocation;

    /** 언리얼 기본 데미지 타입 (점, 광역 등 구분용) */
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    //TSubclassOf<UDamageType> DamageTypeClass;

    /** 게임플레이 태그를 이용한 유연한 데미지 속성 (화염, 냉기, 스턴 유발 등) */
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    //FGameplayTagContainer DamageTags;

    /** 치명타 공격이었는지 여부 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    bool bIsCriticalHit = false;

    // 기본 생성자
    FEPDamageInfo() {}
};