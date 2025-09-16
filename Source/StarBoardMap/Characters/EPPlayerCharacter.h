// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EPCombatCharacterBase.h"
#include "EPPlayerCharacter.generated.h"

// 전방 선언
class UStatComponent;
class UAbilityComponent;
class UCharacterAnimationData;
class USpringArmComponent;
class UCameraComponent;

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


    // ====== ICombatInterface Implementation ======
    // 인터페이스 함수의 기본 구현을 제공합니다.
    virtual void TakeDamage_Implementation(float DamageAmount, AController* InstigatorController, AActor* DamageCauser) override;
    virtual void HandleDeath_Implementation() override;
    virtual UAnimMontage* GetHitReactionMontage_Implementation(EEPHitReactionType HitReactionType) override;



protected:
    virtual void BeginPlay() override;
    virtual void InitializeCharacterData() override;


};
