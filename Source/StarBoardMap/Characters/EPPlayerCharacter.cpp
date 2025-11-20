// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EPPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/EPHealthBlockStatComponent.h"
#include "Components/EPSkillComponent.h"
#include "Data/EPSkillTypes.h"
 // Enhanced Input
#include "EnhancedInputComponent.h" 
#include "EnhancedInputSubsystems.h"

// weapon
#include "Data/EPWeaponTypes.h"

#include "UI/EPHUDWidget.h"

AEPPlayerCharacter::AEPPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // 구체적인 자식 컴포넌트를 생성
    UEPHealthBlockStatComponent* PlayerStatComponent = CreateDefaultSubobject<UEPHealthBlockStatComponent>(TEXT("StatComponent"));

    // 부모의 순수 C++ 포인터에 할당
    StatComponent = PlayerStatComponent;

    // 캐릭터 이동 컴포넌트 설정
    GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터가 이동 방향으로 자연스럽게 회전하도록 설정
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    bUseControllerRotationYaw = false;

    // 스프링 암(카메라 암) 생성 및 설정
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(RootComponent); // 루트 컴포넌트(캡슐)에 부착
    SpringArmComponent->TargetArmLength = 200.0f; // 카메라와의 거리
    SpringArmComponent->bUsePawnControlRotation = true; // 컨트롤러(마우스)의 회전을 스프링 암에 적용

    // 카메라 생성 및 설정
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent); // 스프링 암 끝에 부착
    CameraComponent->bUsePawnControlRotation = false; // 카메라는 스프링 암의 회전을 따라가므로 자체 회전은 비활성화

    // 플레이어 크기 설정
    SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));


    TeamID = 0;
}

void AEPPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    //weapon temp
    UEPWeaponTypes* WeaponData = LoadObject<UEPWeaponTypes>(nullptr, TEXT("/Game/AssetDynamic/Data/Weapon/BP_WeaponTypes.BP_WeaponTypes"));
    if (WeaponData)
    {
        FWeaponInfo Data = WeaponData->GetWeaponInfoByName(FName("Hammer"));
        GetWorld()->SpawnActor<AActor>(Data.WeaponBlueprint, FVector::ZeroVector, FRotator::ZeroRotator);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("weapon data is null -- spawn fail"));
    }


    // user widget
    if (EPHUDWidgetClass)
    {
        EPHUDWidgetInstance = CreateWidget<UEPHUDWidget>(GetWorld(), EPHUDWidgetClass);
        if (EPHUDWidgetInstance)
        {
            EPHUDWidgetInstance->AddToViewport();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("user widget data is null -- spawn fail"));
    }


    if (StatComponent)
    {
        // BeginPlay 시점에 StatComponent의 "체력 변경" 방송을 '구독'합니다.
        StatComponent->OnHealthChanged_Two.AddDynamic(this, &AEPPlayerCharacter::HandleHealthChanged);
    }
}

void AEPPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // Input Action Bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEPPlayerCharacter::Move);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEPPlayerCharacter::Look);

        // Base Attacking
        EnhancedInputComponent->BindAction(BaseAttackAction, ETriggerEvent::Triggered, this, &AEPPlayerCharacter::BaseAttack);
    }
}

void AEPPlayerCharacter::Jump()
{
    // 공격 상태 확인
    if (GetCurrentState() == EEPCharacterState::Attacking)
    {
        return;
    }

    Super::Jump();
}

void AEPPlayerCharacter::Move(const FInputActionValue& Value)
{
    // 공격 상태 확인
    if (GetCurrentState() == EEPCharacterState::Attacking)
    {
        return;
    }

    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AEPPlayerCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AEPPlayerCharacter::BaseAttack(const FInputActionValue& Value)
{
    if (SkillComponent)
    {
        SkillComponent->ActivateSkill(0);
    }
}

float AEPPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 플레이어만의 고유 로직
    if (ActualDamage > 0.f)
    {
        // 카메라 흔들기
        //PlayCameraShake();
        // 피격 시 음성 재생
        //PlayHurtSound();
        // "데미지 50 이상 받기" 같은 퀘스트 진행도 업데이트
        //UpdateQuestProgress();
    }


    return ActualDamage;
}

void AEPPlayerCharacter::OnDied()
{
    // Die 처리 함수 호출
    HandleDeath_Implementation();
}

void AEPPlayerCharacter::CurrentMontagePlay(UAnimMontage* CurrentMontage, EEPCombatMontageType CurrentMontageType)
{
    if (!CurrentMontage) return;

    this->PlayAnimMontage(CurrentMontage);
}

void AEPPlayerCharacter::HandleHealthChanged(float NewHealth, float MaxHealth)
{
    if (EPHUDWidgetInstance)
    {
        // 여기서 최종적으로 위젯의 함수를 호출합니다.
        EPHUDWidgetInstance->UpdateHealthFloat(NewHealth, MaxHealth);
    }
}

