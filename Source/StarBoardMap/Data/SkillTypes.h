// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NiagaraSystem.h"
#include "GameplayTagContainer.h"
#include "SkillTypes.generated.h"

class UAnimMontage;
class UNiagaraSystem;
class USoundBase;
class USkillBase; // 순환 참조(Circular Dependency) 방지용 전방 선언
class AProjectileBase;

/**
 * 
 */

 // 타겟팅 방식
UENUM(BlueprintType)
enum class ETargetType : uint8
{
    None        UMETA(DisplayName = "타겟 없음"),
    Self        UMETA(DisplayName = "자기 자신"),
    Actor       UMETA(DisplayName = "액터 지정"),
    Location    UMETA(DisplayName = "위치 지정"),
    Direction   UMETA(DisplayName = "방향 지정") 
};

// 스킬 범위 형태
UENUM(BlueprintType)
enum class ESkillRangeShape : uint8
{
    // X=길이, Y=폭, Z=높이 
    Box         UMETA(DisplayName = "박스 형태"),
    // X=반지름
    Sphere      UMETA(DisplayName = "구 형태"),
    // X=반지름, Z=절반 높이
    Capsule     UMETA(DisplayName = "캡슐 형태"),
    // X=최대 사거리
    Projectile  UMETA(DisplayName = "발사체 사거리"),
};

// 타겟 데이터
USTRUCT(BlueprintType)
struct FSkillTargetData
{
    GENERATED_BODY()

public:
    // 타겟팅 방식
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target")
    ETargetType TargetType;

    // 타겟 Actor
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target")
    AActor* TargetActor;

    // 공격 위치 (논-타겟)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target")
    FVector TargetLocation;

    // 공격 방향 (논-타겟)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target")
    FVector TargetDirection;
};

// 스킬 범위 데이터
USTRUCT(BlueprintType)
struct FSkillRangeData
{
    GENERATED_BODY()

public: 
    // 공격 범위 형태
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range")
    ESkillRangeShape ShapeType;
    
    // 치수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range")
    FVector Dimensions;
};

// 투사체의 물리적 속성과 시각/청각적 표현을 정의하는 구조체
USTRUCT(BlueprintType)
struct FProjectileData
{
    GENERATED_BODY()

    // 투사체 초기 발사 속도
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float InitialSpeed = 3000.0f;

    // 투사체 최대 속도
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float MaxSpeed = 3000.0f;

    // 투사체에 적용될 중력의 크기(0 : 직선)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float GravityScale = 0.0f; 

    // 투사체의 최대 수명 시간(끝나면 자동으로 소멸), -1 이면 무한 지속
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float LifeSpan = 5.0f;

    // 충돌했을 때 재생할 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TSoftObjectPtr<UNiagaraSystem> ImpactEffect; 

    // 충돌했을 때 재생할 사운드
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TSoftObjectPtr<USoundBase> ImpactSound;

    // 수명이 다해 사라질 때 재생할 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TSoftObjectPtr<UNiagaraSystem> ExpireEffect; 

    // 수명이 다해 사라질 때 재생할 사운드
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TSoftObjectPtr<USoundBase> ExpireSound; 
};

// 스킬 단계 데이터
USTRUCT(BlueprintType)
struct FSkillPhaseData
{
    GENERATED_BODY()

public:
    // 데미지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    float Damage; 

    // 콤보 유효 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    float ComboValidTime;

    // 애니메이션
    //UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    //TSoftObjectPtr<UAnimMontage> Animation;

    // 어떤 애니메이션을 쓸지"에 대한 태그 (CharacterAnimation과 연결) -> CharacterAnimation 에서 검색용
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    FGameplayTag AnimationTag;

    // 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    TSoftObjectPtr<UNiagaraSystem> VFX;

    // 사운드
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    TSoftObjectPtr<USoundBase> SFX;

    // 스킬 범위
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
    FSkillRangeData SkillRange;

    // 발사체 정의 구조체
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    FProjectileData ProjectileInfo;

    // 발사체 블루프린트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    TSoftClassPtr<AProjectileBase> ProjectileClass;

    // 발사체 소멸 시 이펙트 재생 등 위해 기다릴 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    float DeactivationDelay = 0.5f;
    
};

// 데이터 테이블만을 위한 '래퍼' 또는 '껍데기' 구조체
USTRUCT(BlueprintType)
struct FSkillPhaseData_TableRow : public FTableRowBase
{
    GENERATED_BODY()

    // 실제 데이터는 FSkillPhaseData가 모두 가지고 있습니다.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FSkillPhaseData PhaseData;
};