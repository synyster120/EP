// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interfaces/EPPoolable.h"
#include "Data/EPSkillTypes.h"
#include "Core/Subsystems/EPObjectPoolManager.h"
#include "EPProjectileBase.generated.h"

// 전방 선언
class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
struct FEPSkillPhaseData;

UCLASS()
class STARBOARDMAP_API AEPProjectileBase : public AActor, public IEPPoolable // IEPPoolable 상속
{
	GENERATED_BODY()
	
protected:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> MovementComponent;

	// 자신을 관리하는 풀 매니저 저장
	UPROPERTY()
	TSoftObjectPtr<UEPObjectPoolManager> OwnerPool;

	// Initialize()함수 실행했는지 여부 확인 = 초기화 확인
	bool bIsValid;

public:	
	AEPProjectileBase();

	// 스킬로부터 데이터를 받아 초기화하는 메인 함수
	virtual void Initialize(const FEPSkillPhaseData* InPhaseData, AActor* InOwner);

	// --- IEPPoolable 인터페이스 함수 구현 ---
	// 활성화 함수 (스킬에서 Pool로 받고, 활성화할 때 호출)
	virtual void Activate() override;
	// 소멸 준비를 시작하고, 필요한 지연 시간을 반환하는 함수 (Pool에서 호출)
	virtual float BeginDeactivate() override;
	// 실제 비활성화 (Pool에서 호출)
	virtual void Deactivate() override;
	// 현재 활성화 상태인지 확인 (Pool에서 호출)
	virtual bool IsActive() const override { return bIsActive; }
	// 풀 매니저가 자신의 주소를 이 투사체에게 알려주기 위한 함수 (Pool에서 호출)
	virtual void SetOwnerPool(UEPObjectPoolManager* InOwnerPool) override { OwnerPool = InOwnerPool; }




protected:
	virtual void BeginPlay() override;

	// 기존 함수들 (OnHit, OnExpire, Explode 등)의 내부 로직은 Destroy() 대신 Deactivate()를 호출하도록 변경됩니다.
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 생명주기 종료 시 호출 될 함수
	void OnExpire();

	// Manager에 반납 요청 함수
	void OnReturnToPool();


public:	
	virtual void Tick(float DeltaTime) override;

	// 이 투사체의 모든 데이터를 담고 있는 원본 포인터
	// OnHit, OnExpire 등 다른 함수에서 이 데이터를 참조하여 사용합니다.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Data")
	FEPSkillPhaseData PhaseData;

	/** 현재 풀에서 활성화되어 사용 중인지 여부 */
	bool bIsActive;

	/** 수명 타이머 핸들 */
	FTimerHandle LifespanTimer;

};
