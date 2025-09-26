// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EPPoolable.generated.h"

class UEPObjectPoolManager;
struct FEPPoolableObjectInitializer;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEPPoolable : public UInterface
{
	GENERATED_BODY()
};

/**
 *	오브젝트 풀링하기 위한 필요 함수
 */

class STARBOARDMAP_API IEPPoolable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	// 이 객체를 초기화
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Poolable")
	void PoolableInitialize(const FEPPoolableObjectInitializer& Initializer);

	// 이 객체를 활성화
	UFUNCTION()
	virtual void Activate() = 0;

	// 이 객체를 비활성화하기 전, 대기 요청
	UFUNCTION()
	virtual float BeginDeactivate() = 0;

	// 이 객체를 비활성화하고 풀에 반납 준비
	UFUNCTION()
	virtual void Deactivate() = 0;

	// OwnerPool 설정
	UFUNCTION()
	virtual void SetOwnerPool(UEPObjectPoolManager* InOwnerPool) = 0;

	// 현재 활성화 상태인지 여부 반환
	UFUNCTION()
	virtual bool IsActive() const = 0;
};
