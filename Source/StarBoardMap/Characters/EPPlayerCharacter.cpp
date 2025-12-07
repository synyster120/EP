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
#include "Weapon/EP_WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "UI/EPHUDWidget.h"

#include "Items/EPItemBase.h"
#include "Items/EPDroppedItem.h"
#include "Core/Helper/EPItemLibrary.h"

// Instance
#include "Characters/Animation/EPAnimInstance.h"

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

    // 소켓 컴포넌트 생성
    HandMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandMesh"));
    HandMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
    // 소켓에 HandMeshComponent 부착 -> 게임 내내 손을 따라다님
    HandMeshComponent->SetupAttachment(GetMesh(), TEXT("HammerSocket"));


    TeamID = 0;

    // 설정x 했을 경우 대비 - 기본값 (Dropped Item 스폰할 때 기본되는 ItemBase의 블루프린트)
    static ConstructorHelpers::FClassFinder<AEPDroppedItem> DefaultDropItemBP(TEXT("/Game/AssetDynamic/Items/DroppedItems/BP_DroppedItem_Base.BP_DroppedItem_Base_C"));
    if (DefaultDropItemBP.Succeeded())
    {
        DroppedItemClass = DefaultDropItemBP.Class;
    }
}

void AEPPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // weapon temp
    UEPWeaponTypes* WeaponData = LoadObject<UEPWeaponTypes>(nullptr, TEXT("/Game/AssetDynamic/Data/Weapon/BP_WeaponTypes.BP_WeaponTypes"));
    if (WeaponData)
    {
        FWeaponInfo Data = WeaponData->GetWeaponInfoByName(FName("Hammer"));
        OnHandActor = GetWorld()->SpawnActor<AActor>(Data.WeaponBlueprint, FVector::ZeroVector, FRotator::ZeroRotator);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("weapon data is null -- spawn fail"));
    }

    // item capsule
    ItemCollision = Cast<UCapsuleComponent>(GetDefaultSubobjectByName(TEXT("ItemCapsule")));

    ItemCollision->OnComponentBeginOverlap.AddDynamic(this, &AEPPlayerCharacter::OnOverlapBegin);
    ItemCollision->OnComponentEndOverlap.AddDynamic(this, &AEPPlayerCharacter::OnOverlapEnd);


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

    // stat
    if (StatComponent)
    {
        // BeginPlay 시점에 StatComponent의 "체력 변경" 방송을 '구독'합니다.
        StatComponent->OnHealthChanged_Two.AddDynamic(this, &AEPPlayerCharacter::HandleHealthChanged);
    }

    // best item search
    GetWorldTimerManager().SetTimer(CheckItemTimerHandle, this, &AEPPlayerCharacter::CheckNearbyItems, 0.1f, true);
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

        // Drop
        EnhancedInputComponent->BindAction(DropAndPickUpAction, ETriggerEvent::Triggered, this, &AEPPlayerCharacter::DropAndPickUp);
    }
}

void AEPPlayerCharacter::Jump()
{
    // 공격 or 상호작용 상태 확인
    if (GetCurrentState() == EEPCharacterState::Attacking || GetCurrentState() == EEPCharacterState::Interacting)
    {
        return;
    }

    Super::Jump();
}

void AEPPlayerCharacter::Move(const FInputActionValue& Value)
{
    // 공격 or 상호작용 상태 확인
    if (GetCurrentState() == EEPCharacterState::Attacking || GetCurrentState() == EEPCharacterState::Interacting)
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

// 기본 공격
void AEPPlayerCharacter::BaseAttack(const FInputActionValue& Value)
{
    if (SkillComponent && CurrentItemData) // Item 소유중일 때
    {
        if (AEPItemBase* ItemData = Cast<AEPItemBase>(CurrentItemData.GetDefaultObject()))
        {
            if (ItemData->ItemType == EEPItemType::Equipment) // 소유중인 Item 이 "장비"라면
            {
                SkillComponent->ActivateSkill(0); // 공격
            }
        }
    }
}

// 소유한 Item 제거
void AEPPlayerCharacter::RemoveFromCharacter()
{
    if (HandMeshComponent)
    {
        // 부착된 Mesh 제거
        HandMeshComponent->SetStaticMesh(nullptr);
    }

    // 현재 들고 있는 아이템 데이터 제거
    CurrentItemData = nullptr;

}

// 가장 근처 Item 반환
AEPDroppedItem* AEPPlayerCharacter::FindBestInteractable()
{
    if (NearbyItems.Num() == 0) return nullptr;

    AEPDroppedItem* BestItem = nullptr;
    float MaxScore = -1.0f; // 점수 (-1 ~ 1) = 높을 수록 가까움

    // 카메라 위치/방향 가져오기
    FVector CamLoc;
    FRotator CamRot;
    GetController()->GetPlayerViewPoint(CamLoc, CamRot);
    FVector CamDir = CamRot.Vector();

    // TSet을 순회하며 삭제
    for (auto It = NearbyItems.CreateIterator(); It; ++It)
    {
        AActor* ItemActor = *It; // 현재 이터레이터가 가리키는 액터

        // 유효성 검사 & 청소 (Garbage Collection)
        if (!IsValid(ItemActor))
        {
            // 안전하게 삭제
            It.RemoveCurrent();
            continue;
        }

        AEPDroppedItem* DroppedItem = Cast<AEPDroppedItem>(ItemActor);
        if (!DroppedItem) continue;

        // 방향 벡터 계산 (카메라 -> 아이템)
        FVector DirectionToItem = (DroppedItem->GetActorLocation() - CamLoc).GetSafeNormal();

        // 내적 (Dot Product)
        float DotResult = FVector::DotProduct(CamDir, DirectionToItem);

        // 시야각 필터링 (약 60도 내외)
        if (DotResult < 0.5f) continue;

        // 점수 계산 (내적값 우선)
        if (DotResult > MaxScore)
        {
            MaxScore = DotResult;
            BestItem = DroppedItem;
        }
    }

    return BestItem;
}

// Item 부착 및 회전 오프셋 적용
void AEPPlayerCharacter::EquipItem(TSubclassOf<AEPItemBase> NewItemClass)
{
    AEPItemBase* DefaultItem = Cast<AEPItemBase>(NewItemClass->GetDefaultObject());
    if (!DefaultItem) return;
    if (!HandMeshComponent) return;

    // 메쉬 교체
    if (DefaultItem->EquippedMesh)
    {
        HandMeshComponent->SetStaticMesh(DefaultItem->EquippedMesh);

        // < 부착되는 Item의 Transform 조절 > - 월드 Drop Scale 과 부착된 StaticMesh 사이즈 동일하게 하기위함
        FTransform FinalTransform = DefaultItem->EquippedOffset;

        // 오프셋(오차) Scale과 아이템의 기본 Scale(ItemScale)을 곱함
        FVector CombinedScale = DefaultItem->ItemScale * DefaultItem->EquippedOffset.GetScale3D();
        // 합쳐진 Scale 반영
        FinalTransform.SetScale3D(CombinedScale);

        // 최종 Transform 한번에 적용
        HandMeshComponent->SetRelativeTransform(FinalTransform);
    }
}

// 상호작용 key('E') 바인딩 함수 (줍기/놓기)
void AEPPlayerCharacter::DropAndPickUp(const FInputActionValue& Value)
{
    if (CurrentState == EEPCharacterState::Interacting) // 이미 상호작용 중이라면 탈출
    {
        return;
    }

    if (CurrentItemData) {
        CurrentState = EEPCharacterState::Interacting;
        //UE_LOG(LogTemp, Warning, TEXT("Player --- drop"));
        if (AEPItemBase* DefaultItem = Cast<AEPItemBase>(CurrentItemData.GetDefaultObject()))
        {
            this->PlayAnimationByTag(DefaultItem->GetDropInteractionTag()); // [ Drop ] 몽타주 플레이
        }
    }
    else {
        //UE_LOG(LogTemp, Warning, TEXT("Player --- pickup"));
        BestDroppedItem = FindBestInteractable();

        if (BestDroppedItem == nullptr) {
            UE_LOG(LogTemp, Warning, TEXT("ClosestActor is Nullptr"));
            return;
        }

        CurrentState = EEPCharacterState::Interacting;

        CurrentItemData = BestDroppedItem->GetOriginalItemClass();
        if (AEPItemBase* DefaultItem = Cast<AEPItemBase>(CurrentItemData.GetDefaultObject()))
        {
            this->PlayAnimationByTag(DefaultItem->GetPickupInteractionTag()); // [ PickUp ] 몽타주 플레이
        }
    }
}

// Item 해제 함수 (AnimNotify 에서 호출)
void AEPPlayerCharacter::Drop()
{
    if (CurrentItemData)
    {
        AEPItemBase* DefaultItem = Cast<AEPItemBase>(CurrentItemData->GetDefaultObject());
        if (!DefaultItem) return;

        // ABP 상태 변경
        UpdateAnimationState(DefaultItem->ItemAnimtionType, false);

        // 손에 있는 메쉬 제거
        if (HandMeshComponent)
        {
            HandMeshComponent->SetStaticMesh(nullptr);
        }

        // 위치 계산 (내 앞쪽 + 바닥)
        FVector DropLocation = GetActorLocation() + (GetActorForwardVector() * 50.0f);
    
        // (선택) 바닥 보정을 위해 GetGroundLocation(LootComponent)을 여기서 사용 --> 만약 사용한다면 헬퍼 함수로 옮겨서 사용

        // Dropped Item 을 스폰(헬퍼 함수)
        UEPItemLibrary::SpawnDroppedItem(
            this,
            DroppedItemClass,  // 플레이어가 알고 있는 껍데기 클래스
            CurrentItemData,   // 지금 들고 있던 아이템 데이터
            DropLocation,
            1
        );

        // 데이터 비우기
        CurrentItemData = nullptr;
    }
}

// Item 부착 함수 (AnimNotify 에서 호출)
void AEPPlayerCharacter::PickUp()
{
    if (CurrentItemData)
    {
        // Item 부착
        EquipItem(CurrentItemData);

        AEPItemBase* DefaultItem = Cast<AEPItemBase>(CurrentItemData->GetDefaultObject());
        if (!DefaultItem) return;

        // ABP 상태 변경
        UpdateAnimationState(DefaultItem->ItemAnimtionType, true);

        // 월드에 Drop 되어있던 Item 삭제
        if (IsValid(BestDroppedItem))
        {
            // 액터 파괴 명령
            BestDroppedItem->DestroyItem(); // 유언 전달 및 파괴
            BestDroppedItem = nullptr; // 초기화
        }
    }
}

void AEPPlayerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;
  
    NearbyItems.Add(OtherActor);
}

void AEPPlayerCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor || OtherActor == this) return;

    NearbyItems.Remove(OtherActor);
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
        // 여기서 최종적으로 위젯의 함수를 호출
        EPHUDWidgetInstance->UpdateHealthFloat(NewHealth, MaxHealth);
    }
}

// 애니메이션BP 상태 변경하는 함수
void AEPPlayerCharacter::UpdateAnimationState(EEPItemAnimType CurrentAnimType, bool bCurrentUpdateState)
{
    USkeletalMeshComponent* MeshComp = GetMesh();
    if (!MeshComp) return;

    // 내가 만든 클래스로 캐스팅 (형변환)
    UEPAnimInstance* AnimInstance = Cast<UEPAnimInstance>(MeshComp->GetAnimInstance());

    if (AnimInstance)
    {
        switch (CurrentAnimType)
        {
        case EEPItemAnimType::None:
            UE_LOG(LogTemp, Warning, TEXT("EEPItemAnimType::None - mapping bool type val is null"));
            break;
        case EEPItemAnimType::Hammer:
            AnimInstance->IsGotHammer_C = bCurrentUpdateState;
            break;
        case EEPItemAnimType::Sword:
            UE_LOG(LogTemp, Warning, TEXT("EEPItemAnimType::Sword - mapping bool type val is null"));
            break;
        case EEPItemAnimType::Potion:
            UE_LOG(LogTemp, Warning, TEXT("EEPItemAnimType::Potion - mapping bool type val is null"));
            break;
        case EEPItemAnimType::Object:
            AnimInstance->IsGotPotion_C = bCurrentUpdateState;
            break;
        case EEPItemAnimType::Gun:
            UE_LOG(LogTemp, Warning, TEXT("EEPItemAnimType::Gun - mapping bool type val is null"));
            break;
        default:
            UE_LOG(LogTemp, Warning, TEXT("EEPItemAnimType value is null"));
            break;
        }
    }
}

// 0.1초마다 호출되는 함수
void AEPPlayerCharacter::CheckNearbyItems()
{
    // 상호작용 중일 때는 탈출
    if (CurrentState == EEPCharacterState::Interacting)
    {
        UE_LOG(LogTemp, Warning, TEXT("CheckNearbyItems -- fail :: player is interacting"));
        return;
    }

    // 가장 좋은 아이템 찾기
    AEPDroppedItem* NewBestItem = FindBestInteractable();

    // 상태가 변했는지 확인 (Dirty Check)
    if (NewBestItem != BestDroppedItem)
    {
        // 기존 아이템 UI 끄기
        if (BestDroppedItem && IsValid(BestDroppedItem))
        {
            BestDroppedItem->HideInteractionUI();
        }

        // 변수 업데이트
        BestDroppedItem = NewBestItem;

        // 새 아이템 UI 켜기
        if (BestDroppedItem && IsValid(BestDroppedItem))
        {
            BestDroppedItem->ShowInteractionUI();
        }
    }
}
