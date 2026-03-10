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

// Tag
#include "Core/EPGameplayTags.h"

// respawn
#include "Core/Subsystems/EPRespawnSubsystem.h"

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
        // 초기 상태 업데이트
        FEPHealthInfo CurrentHealthInfo = StatComponent->GetHealthInfo();
        float CurrentHealth = CurrentHealthInfo.CurrentHealth;
        float MaxHealth = CurrentHealthInfo.MaxHealth;
        HandleHealthChanged(CurrentHealth, MaxHealth);
    }

    // 스켈레탈 메시의 0번 슬롯 머티리얼을 다이내믹으로 변환하여 생성 및 적용
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        // CreateAndSetMaterialInstanceDynamic는 머티리얼을 복사하고 자동으로 메시에 다시 입혀줍니다.
        //DissolveMID = MeshComp->CreateAndSetMaterialInstanceDynamic(0);

        // (참고) 만약 캐릭터의 머티리얼 슬롯이 여러 개라면 for문으로 모두 변환해야함
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
    if (SkillComponent && OwnedItemData) // Item 소유중일 때
    {
        if (AEPItemBase* ItemData = Cast<AEPItemBase>(OwnedItemData.GetDefaultObject()))
        {
            if (ItemData->ItemType == EEPItemType::Equipment) // 소유중인 Item 이 "장비"라면
            {
                SkillComponent->ProcessSkillInput(0); // 공격
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
    OwnedItemData = nullptr;

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
    if (!GetController()) return nullptr;
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

    if (OwnedItemData) {
        CurrentState = EEPCharacterState::Interacting;
        UE_LOG(LogTemp, Warning, TEXT("Player --- drop"));
        if (AEPItemBase* DefaultItem = Cast<AEPItemBase>(OwnedItemData.GetDefaultObject()))
        {
            // 몽타주 종료 델리게이트 바인딩
            FOnMontageEnded EndDelegate;
            EndDelegate.BindUObject(this, &AEPPlayerCharacter::OnInteractionMontageEnded, DefaultItem->GetDropInteractionTag());

            this->PlayAnimationByTag(DefaultItem->GetDropInteractionTag(), EndDelegate); // [ Drop ] 몽타주 플레이
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Player --- pickup"));
        BestDroppedItem = FindBestInteractable();

        if (BestDroppedItem == nullptr) {
            UE_LOG(LogTemp, Warning, TEXT("ClosestActor is Nullptr"));
            return;
        }

        CurrentState = EEPCharacterState::Interacting;

        TargetDroppedItem = BestDroppedItem;
        TargetItemData = BestDroppedItem->GetOriginalItemClass();
        if (AEPItemBase* DefaultItem = Cast<AEPItemBase>(TargetItemData.GetDefaultObject()))
        {
            // 몽타주 종료 델리게이트 바인딩
            FOnMontageEnded EndDelegate;
            EndDelegate.BindUObject(this, &AEPPlayerCharacter::OnInteractionMontageEnded, DefaultItem->GetPickupInteractionTag());

            this->PlayAnimationByTag(DefaultItem->GetPickupInteractionTag(), EndDelegate); // [ PickUp ] 몽타주 플레이
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("pickup - get tager item data is not epitembase --> pickup fail"));
        }
    }
}

// 상호작용 Montage 종료 시점 바인딩 함수
void AEPPlayerCharacter::OnInteractionMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGameplayTag ActionTag)
{
    // 비정상 종료 확인
    if (bInterrupted && ActionTag.MatchesTag(FEPGameplayTags::Get().Tag_InputUserSettings))
    {
        // 상태를 다시 Idle로 복구
        // 단, 이미 사망했거나 다른 특수 상태가 아니라면
        if (CurrentState == EEPCharacterState::Interacting)
        {
            CurrentState = EEPCharacterState::Idle;

            // Pickup
            if (TargetItemData != nullptr && ActionTag.MatchesTag(FEPGameplayTags::Get().Tag_InputUserSettings_PickUp))
            {
                UE_LOG(LogTemp, Warning, TEXT("[TEST2] interaction montage end delegate function - interaction is pickup and target item reset"));
                TargetItemData = nullptr;
                return;
            }

            // Drop
            if (ActionTag.MatchesTag(FEPGameplayTags::Get().Tag_InputUserSettings_Drop))
            {
                UE_LOG(LogTemp, Warning, TEXT("[TEST2] interaction montage end delegate function - interaction is drop"));
                return;
            }
        }
    }

    // ----------------- 정상 종료 -----------------
    // Pickup
    if (ActionTag.MatchesTag(FEPGameplayTags::Get().Tag_InputUserSettings_PickUp))
    {

    }

    // Drop
    if (ActionTag.MatchesTag(FEPGameplayTags::Get().Tag_InputUserSettings_Drop))
    {

    }


    //if (CurrentItemData) {
    //    if (AEPItemBase* DefaultItem = Cast<AEPItemBase>(CurrentItemData.GetDefaultObject()))
    //    {
    //        //if (Montage == CurrentInteractionMontage)
    //        if (ActionTag.MatchesTag(FEPGameplayTags::Get().Tag_InputUserSettings))
    //        {
    //            // 상태를 다시 Idle로 복구
    //            // 단, 이미 사망했거나 다른 특수 상태가 아니라면
    //            if (CurrentState == EEPCharacterState::Interacting)
    //            {
    //                CurrentState = EEPCharacterState::Idle;
    //            }
    //        }
    //        else
    //        {
    //            UE_LOG(LogTemp, Warning, TEXT("pickup&drop montage end binding function :: montage param : %s, current montage : %s"), *Montage->GetName(), *CurrentInteractionMontage.GetName());
    //        }
    //    }
    //}
}

// Item 해제 함수 (AnimNotify 에서 호출)
void AEPPlayerCharacter::Drop()
{
    if (OwnedItemData)
    {
        AEPItemBase* DefaultItem = Cast<AEPItemBase>(OwnedItemData->GetDefaultObject());
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
            OwnedItemData,   // 지금 들고 있던 아이템 데이터
            DropLocation,
            1
        );

        // 데이터 비우기
        OwnedItemData = nullptr;
    }
}

// Item 부착 함수 (AnimNotify 에서 호출)
void AEPPlayerCharacter::PickUp()
{
    if (TargetItemData)
    {
        AEPItemBase* DefaultItem = Cast<AEPItemBase>(TargetItemData->GetDefaultObject());
        if (!DefaultItem) return;

        // Item 부착
        EquipItem(TargetItemData);

        // Item 데이터 이전
        OwnedItemData = TargetItemData; // 소유 데이터로 저장
        TargetItemData = nullptr; // 타겟 데이터 제거

        // ABP 상태 변경
        UpdateAnimationState(DefaultItem->ItemAnimtionType, true);

        // 월드에 Drop 되어있던 Item 삭제
        if (IsValid(TargetDroppedItem)) // BestDroppedItem 을 따로 저장해두는 게 나을 듯 (언제든 바뀔 수 있는 변수이기 때문)
        {
            // 액터 파괴 명령
            TargetDroppedItem->DestroyItem(); // 유언 전달 및 파괴
            TargetDroppedItem = nullptr; // 초기화
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

    // 상태 변경
    CurrentState = EEPCharacterState::Dead;
}

void AEPPlayerCharacter::HandleDeath_Implementation()
{
    Super::HandleDeath_Implementation();


    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        // 기존 입력 무시
        DisableInput(PlayerController);


        // 입력 모드를 UI 전용으로 변경 (게임 오버 위젯이 떴다고 가정)
        // 이렇게 해야 WASD를 눌러도 캐릭터가 안 움직이고 마우스만 나옴
        FInputModeUIOnly InputMode;

        // (선택) 포커스를 맞출 위젯 설정. 없으면 nullptr
        // InputMode.SetWidgetToFocus(...); 

        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = true; // 마우스 커서 보이기
    }

}

void AEPPlayerCharacter::CurrentMontagePlay(UAnimMontage* CurrentMontage, EEPCombatMontageType CurrentMontageType)
{
    if (!CurrentMontage) return;

    //this->PlayAnimMontage(CurrentMontage);
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


// --- RESPAWN ---
void AEPPlayerCharacter::StartRespawnSequence()
{
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController && PlayerController->PlayerCameraManager)
    {
        // 입력 차단
        DisableInput(PlayerController);

        // 물리적 움직임 및 중력 제거 (허공에 둥둥 띄우기)
        UCharacterMovementComponent* MovementComp = GetCharacterMovement();
        if (MovementComp)
        {
            // 떨어지던 가속도 즉시 0으로 초기화
            MovementComp->StopMovementImmediately();

            // 물리 연산 정지
            MovementComp->SetMovementMode(MOVE_None); // 이동 모드 'None'

            // 캡슐이 바닥을 뚫거나 다른 것과 부딪히지 않도록 충돌체 무시 처리 (선택 사항)
            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        // 애니메이션 시간 정지 (현재 포즈 그대로 굳히기)
        USkeletalMeshComponent* MeshComp = GetMesh();
        if (MeshComp)
        {
            MeshComp->bPauseAnims = true; // 애니메이션 업데이트 정지
        }

        // 머티리얼 디졸브 연출 시작 (블루프린트가 수행)
        PlayDissolveEffect(); // --> OnDissolveFinished()
    }
}

//  fade out & respawn (블루프린트에서 호출)
void AEPPlayerCharacter::OnDissolveFinished()
{
    // 이제 캐릭터가 완전히 투명해졌으므로, 카메라 페이드 아웃 시작
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController && PlayerController->PlayerCameraManager)
    {
        // 카메라 Fade Out 실행 - StartCameraFade(FromAlpha, ToAlpha, Duration, Color, bFadeAudio, bHoldWhenFinished)
        PlayerController->PlayerCameraManager->StartCameraFade(0.f, 1.f, 1.0f, FLinearColor::Black, false, true);

        // 페이드 아웃이 끝나는 1초 뒤에 텔레포트 진행
        FTimerHandle TeleportTimer;
        GetWorld()->GetTimerManager().SetTimer(TeleportTimer, this, &AEPPlayerCharacter::RequestTeleportToSubsystem, 1.0f, false);
    }
}

// SpawnSubsystem에 텔레포트 요청
void AEPPlayerCharacter::RequestTeleportToSubsystem()
{
    // 통해 텔레포트 실행
    if (UGameInstance* UGameInstance = GetGameInstance())
    {
        if (UEPRespawnSubsystem* RespawnSub = UGameInstance->GetSubsystem<UEPRespawnSubsystem>())
        {
            // 텔레포트 완료타이밍 바인딩
            RespawnSub->OnPlayerRespawned.AddDynamic(this, &AEPPlayerCharacter::ExecuteFadeIn);

            RespawnSub->RespawnPlayer(this);
        }
    }
}

// 카메라 fade in
void AEPPlayerCharacter::ExecuteFadeIn()
{
    // 물리 연산 및 충돌 복구
    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    if (MovementComp)
    {
        // 다시 걸어다닐 수 있게 걷기 모드로 변경
        MovementComp->SetMovementMode(MOVE_Walking);
        // 바닥 찾음
        MovementComp->Velocity = FVector::ZeroVector;
        MovementComp->bForceNextFloorCheck = true;


        // 충돌체 다시 활성화 (QueryAndPhysics가 캐릭터 기본값)
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    // 애니메이션 굳은 것 풀어주기
    USkeletalMeshComponent* MeshComp = GetMesh();
    if (MeshComp)
    {
        MeshComp->bPauseAnims = false; // 애니메이션 다시 재생 시작

        // 애니메이션 상태머신 새로고침 지시
        MeshComp->TickAnimation(0.f, false);
        MeshComp->RefreshBoneTransforms();
    }

    // fade in 
    FTimerHandle FadeInTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(FadeInTimerHandle, [this]()
        {
            APlayerController* PlayerController = Cast<APlayerController>(GetController());
            if (PlayerController && PlayerController->PlayerCameraManager)
            {
                // Fade In 실행: 불투명 검은색(1)에서 투명(0)으로 1초 동안 페이드
                PlayerController->PlayerCameraManager->StartCameraFade(1.f, 0.f, 1.0f, FLinearColor::Black, false, false);

                // 입력 권한 돌려주기
                EnableInput(PlayerController);
            }
        }, 0.15f, false); // 0.15초의 안정화 시간 확보

}
