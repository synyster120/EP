// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EPCombatCharacterBase.h"
#include "Data/EPItemTypes.h"
#include "EPPlayerCharacter.generated.h"

// 전방 선언
class UStatComponent;
class UAbilityComponent;
class UCharacterAnimationData;
class USpringArmComponent;
class UCameraComponent;

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UCapsuleComponent;

class UEPHUDWidget;

class AEPItemBase;
class AEPDroppedItem;
/**
 * 
 */
UCLASS()
class STARBOARDMAP_API AEPPlayerCharacter : public AEPCombatCharacterBase
{
	GENERATED_BODY()

public:
    AEPPlayerCharacter();

    // 3인칭 카메라를 위한 스프링 암과 카메라 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> CameraComponent;

	// Item 부착 Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> HandMeshComponent;

    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnDied() override;
	virtual void CurrentMontagePlay(UAnimMontage* CurrentMontage, EEPCombatMontageType CurrentMontageType) override;

	virtual void HandleDeath_Implementation() override;

	virtual void HandleHealthChanged(float NewHealth, float MaxHealth) override;

	UFUNCTION()
	void OnInteractionMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGameplayTag ActionTag);

	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetHandMeshComponent() { return HandMeshComponent; };

protected:
    // 입출력
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Bast Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BaseAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DropAndPickUpAction;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UEPHUDWidget> EPHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UEPHUDWidget> EPHUDWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	TSubclassOf<AEPDroppedItem> DroppedItemClass;

protected:
    virtual void BeginPlay() override;
    //virtual void InitializeCharacterData() override;
	// 
	 // APawn으로부터 상속받은, 입력 컴포넌트 설정 함수를 재정의
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Jump() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for BaseAttacking input */
	void BaseAttack(const FInputActionValue& Value);

	void DropAndPickUp(const FInputActionValue& Value);

	// 소켓 Mesh 지우기
	void RemoveFromCharacter();

	// Items 가장 가까운 거리 계산
	AEPDroppedItem* FindBestInteractable();

	// Item 을 소켓에 부착, 회전 오프셋 적용
	void EquipItem(TSubclassOf<AEPItemBase> NewItemClass);

	// 애니메이션BP 상태 변경하는 함수
	void UpdateAnimationState(EEPItemAnimType CurrentAnimType, bool bCurrentUpdateState);

	// BestDroppedItem 검색 timer
	UPROPERTY()
	FTimerHandle CheckItemTimerHandle;

	void CheckNearbyItems();

	// 가장 가까운 Dropped Item
	UPROPERTY()
	AEPDroppedItem* BestDroppedItem = nullptr;

	UPROPERTY()
	AActor* OnHandActor;

	// 실제 소유중인 Item Data
	UPROPERTY()
	TSubclassOf<AEPItemBase> CurrentItemData;

	UPROPERTY()
	UCapsuleComponent* ItemCollision;

	UPROPERTY()
	TSet<AActor*> NearbyItems;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	public:
		void Drop();
		void PickUp();

};
