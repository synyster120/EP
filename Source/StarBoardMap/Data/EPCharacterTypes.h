// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EPCharacterTypes.generated.h"

// 전방 선언
class UEPSkillDataAsset;

/**
 * 
 */

// AI의 행동 상태
UENUM(BlueprintType)
enum class EEPAIState : uint8
{
	Patrolling UMETA(DisplayName = "대기or순찰"), // 대기or순찰
	Chasing UMETA(DisplayName = "추격"), // 추격
	Attacking UMETA(DisplayName = "공격"), // 공격
	Repositioning UMETA(DisplayName = "거리조절"), // 거리조절
	Fleeing UMETA(DisplayName = "도망"), // 도망

};

// 체력 시스템 타입을 정의하는 열거형
UENUM(BlueprintType)
enum class EEPHealthType : uint8
{
	HealthBar UMETA(DisplayName = "체력 바(float)"),
	HealthBlock UMETA(DisplayName = "체력 칸(int)")
};

// 기본 스탯
USTRUCT(BlueprintType)
struct FEPBaseStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	float MaxHealth = 100.0f;

	// 사정거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	float AttackRange = 10.0f;

	// 공격속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	float AttackSpeed = 10.0f;

	// 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	float MovementSpeed = 10.0f;

	// 스킬 종류(Key)
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TArray<TSoftObjectPtr<UEPSkillDataAsset>> Skills;*/
};

// 플레이어 스탯 (기본 스탯 상속)
USTRUCT(BlueprintType)
struct FEPPlayerStat : public FEPBaseStat
{
	GENERATED_BODY()

public:
};

// 적 스탯 (기본 스탯 상속)
USTRUCT(BlueprintType)
struct FEPEnemyStat : public FEPBaseStat
{
	GENERATED_BODY()

public:
	// 인지 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float PerceptionRadius = 0.0f;

	// 인지 놓치는 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float LosePerceptionRadius = 0.0f;
};

// 체력 데이터
USTRUCT(BlueprintType)
struct FEPHealthInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EEPHealthType HealthType;

	UPROPERTY(BlueprintReadOnly)
	float HealthRatio = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float MaxHealth = 0.f;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentBlocks = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxBlocks = 0;
};