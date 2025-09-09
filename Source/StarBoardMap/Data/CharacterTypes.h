// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterTypes.generated.h"

// 전방 선언
class USkillBase;

/**
 * 
 */

// AI의 행동 상태
UENUM(BlueprintType)
enum class EAIState : uint8
{
	Patrolling UMETA(DisplayName = "대기or순찰"), // 대기or순찰
	Chasing UMETA(DisplayName = "추격"), // 추격
	Attacking UMETA(DisplayName = "공격"), // 공격
	Repositioning UMETA(DisplayName = "거리조절"), // 거리조절
	Fleeing UMETA(DisplayName = "도망"), // 도망

};

// 기본 스탯
USTRUCT(BlueprintType)
struct FBaseStat : public FTableRowBase
{
	GENERATED_BODY()

public:
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TArray<TSoftObjectPtr<USkillBase>> Skills;
};

// 플레이어 스탯 (기본 스탯 상속)
USTRUCT(BlueprintType)
struct FPlayerStat : public FBaseStat
{
	GENERATED_BODY()

public:
	// 체력(칸)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	int Health = 3;
};

// 적 스탯 (기본 스탯 상속)
USTRUCT(BlueprintType)
struct FEnemyStat : public FBaseStat
{
	GENERATED_BODY()

public:
	// 체력(칸)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	float Health = 100.0f;

	// 인지 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float PerceptionRadius = 0.0f;

	// 인지 놓치는 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float LosePerceptionRadius = 0.0f;
};