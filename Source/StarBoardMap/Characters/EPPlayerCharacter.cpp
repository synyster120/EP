// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EPPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/EPStatComponent.h"
#include "Components/EPSkillComponent.h"

AEPPlayerCharacter::AEPPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // 캐릭터 이동 컴포넌트 설정
    GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터가 이동 방향으로 자연스럽게 회전하도록 설정
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    bUseControllerRotationYaw = false;

    // 스프링 암(카메라 암) 생성 및 설정
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(RootComponent); // 루트 컴포넌트(캡슐)에 부착
    SpringArmComponent->TargetArmLength = 400.0f; // 카메라와의 거리
    SpringArmComponent->bUsePawnControlRotation = true; // 컨트롤러(마우스)의 회전을 스프링 암에 적용

    // 카메라 생성 및 설정
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent); // 스프링 암 끝에 부착
    CameraComponent->bUsePawnControlRotation = false; // 카메라는 스프링 암의 회전을 따라가므로 자체 회전은 비활성화

}

void AEPPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AEPPlayerCharacter::InitializeCharacterData()
{
    Super::InitializeCharacterData();
}

void AEPPlayerCharacter::TakeDamage_Implementation(float DamageAmount, AController* InstigatorController, AActor* DamageCauser)
{
    Super::TakeDamage_Implementation(DamageAmount, InstigatorController, DamageCauser);

    // 할 수 있는 것들
    // 


    // 1. [조회] StatComponent로부터 필요한 모든 스탯 정보를 'const' Get 함수로 안전하게 가져온다.
    //const float Defense = StatComponent->GetDefense(); // GetDefense() 함수가 있다고 가정
    //const float FireResistance = StatComponent->GetSecondaryStat(ESecondaryStatType::FireResistance);

    //// 2. [계산] 가져온 데이터를 바탕으로 이 캐릭터 고유의 데미지 공식을 적용한다.
    //float FinalDamage = DamageAmount * (100 / (100 + Defense)); // 방어력 공식 예시
    //FinalDamage *= (1.0f - FMath::Clamp(FireResistance, 0.0f, 100.0f) / 100.0f); // 저항력(%) 적용 예시

    // 피격 타입에 맞는 애니메이션 검색
    UAnimMontage* HitMontage = GetHitReactionMontage_Implementation(EEPHitReactionType::Heavy);
    PlayAnimMontage(HitMontage);

    // 최종 계산된 값으로 StatComponent에 데이터 수정 명령
    //StatComponent->ApplyDamage(FinalDamage);
}

void AEPPlayerCharacter::HandleDeath_Implementation()
{
    Super::HandleDeath_Implementation();

}

UAnimMontage* AEPPlayerCharacter::GetHitReactionMontage_Implementation(EEPHitReactionType HitReactionType)
{
    Super::GetHitReactionMontage_Implementation(HitReactionType);

	return nullptr;
}

