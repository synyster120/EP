
#include "Characters/EPCharacterBase.h"
#include "Core/Helper/EPAsyncLoadHelper.h"
#include "Data/EPCharacterAnimationData.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Characters/EPCombatCharacterBase.h"
#include "Components/EPMovementLockComponent.h"

#include "Core/EPGameplayTags.h"

AEPCharacterBase::AEPCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false;
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);

    MovementLockComponent = CreateDefaultSubobject<UEPMovementLockComponent>(TEXT("MovementLockComponent"));

}

// 애니메이션 검색 후 Play, 바인딩 데이터를 파라미터로 포함
void AEPCharacterBase::PlayAnimationByTag(FGameplayTag NewTag, const FOnMontageEnded& OnMontageEndedDelegate)
{
    if (!AnimDataAsset) return;

    // TMap에서 태그에 맞는 몽타주 검색
    FEPActionAnimationInfo* FoundInfo = AnimDataAsset->ActionMap.Find(NewTag);
    TSoftObjectPtr<UAnimMontage> MontageToPlay = nullptr;
    EEPCombatMontageType FoundMontageType = EEPCombatMontageType::None;

    if (NewTag.MatchesTag(FEPGameplayTags::Get().Tag_State_Dead)) // EPGameplayTags 에 정의한 Tag 가져옴
    {
        MontageToPlay = AnimDataAsset->DeadAnimationInfo.Montage; // 몽타주 - 별도 변수 활용
        FoundMontageType = AnimDataAsset->DeadAnimationInfo.TargetState; // 상태
    }
    else if (FoundInfo)
    {
        MontageToPlay = FoundInfo->Montage; // 몽타주
        FoundMontageType = FoundInfo->TargetState; // 상태
    }

    // 예외 처리: 찾지 못하면 Default 재생
    if (MontageToPlay.IsNull())
    {
        MontageToPlay = AnimDataAsset->DefaultHitInfo.Montage;
        FoundMontageType = AnimDataAsset->DefaultHitInfo.TargetState; // 상태
        UE_LOG(LogTemp, Warning, TEXT("Tag [%s] not found! Playing Default."), *NewTag.ToString());
    }

    // 비동기 몽타주 재생
    if (!MontageToPlay.IsNull())
    {
        // 비동기 로드 로드 (헬퍼 사용)
        UEPAsyncLoadHelper::RequestAsyncLoad<UAnimMontage>(MontageToPlay,
            [this, NewTag, FoundMontageType, OnMontageEndedDelegate](UAnimMontage* LoadedMontage) // 람다의 파라미터로 로드된 몽타주가 들어옴
            {
                if (!LoadedMontage) return;

                UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
                if (!AnimInstance) return;

                // 가져온 델리게이트 바인딩
                FOnMontageEnded EndDelegate = OnMontageEndedDelegate;

                // 몽타주 재생 및 상태 업데이트
                AnimInstance->Montage_Play(LoadedMontage);
                AnimInstance->Montage_SetEndDelegate(EndDelegate, LoadedMontage);

                // CombatCharacter 전용 로직 처리
                if (AEPCombatCharacterBase* CombatCharacter = Cast<AEPCombatCharacterBase>(this))
                {
                    CombatCharacter->SetCurrentInteractionMontage(LoadedMontage);
                    CombatCharacter->CurrentMontagePlay(LoadedMontage, FoundMontageType);
                }

                //CurrentActionTag = NewTag;
            }
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("tag base anim montage fide is fail"));
    }
}

void AEPCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEPCharacterBase::InitializeCharacterData()
{
	// 초기화 로직
}

// 몽타주 종료 타이밍 바인딩 함수
void AEPCharacterBase::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGameplayTag AssociatedTag)
{
    // 애니메이션이 끝나면 방송, 플레이한 몽타주를 인수로 전달
    OnActionEnded.Broadcast(AssociatedTag);
}

// 몽타주 비동기 재생
void AEPCharacterBase::PlayAnimationByTag(FGameplayTag NewTag)
{
    if (!AnimDataAsset) return;

    // TMap에서 태그에 맞는 몽타주 검색
    FEPActionAnimationInfo* FoundInfo = AnimDataAsset->ActionMap.Find(NewTag);
    TSoftObjectPtr<UAnimMontage> MontageToPlay = nullptr; 
    EEPCombatMontageType FoundMontageType = EEPCombatMontageType::None;

    bool bIsNewTagValid = NewTag.IsValid();
    bool bIsNativeTagValid = FEPGameplayTags::Get().Tag_State_Dead.IsValid();
    UE_LOG(LogTemp, Error, TEXT("NewTag Valid: %d, NativeTag Valid: %d"), bIsNewTagValid, bIsNativeTagValid);
    UE_LOG(LogTemp, Error, TEXT("NewTag Valid: %s, NativeTag Valid: %s"), *NewTag.ToString(), *FEPGameplayTags::Get().Tag_State_Dead.ToString());


    if (NewTag.MatchesTag(FEPGameplayTags::Get().Tag_State_Dead)) // EPGameplayTags 에 정의한 Tag 가져옴
    {
        //UE_LOG(LogTemp, Warning, TEXT("Tag [%s] found! Playing dead montage."), *NewTag.ToString());
        MontageToPlay = AnimDataAsset->DeadAnimationInfo.Montage; // 몽타주 - 별도 변수 활용
        FoundMontageType = AnimDataAsset->DeadAnimationInfo.TargetState; // 상태
    }
    else if (FoundInfo)
    {
        MontageToPlay = FoundInfo->Montage; // 몽타주
        FoundMontageType = FoundInfo->TargetState; // 상태
    }

    // 예외 처리: 찾지 못하면 Default 재생
    if (MontageToPlay.IsNull())
    {
        MontageToPlay = AnimDataAsset->DefaultHitInfo.Montage;
        FoundMontageType = AnimDataAsset->DefaultHitInfo.TargetState; // 상태
        UE_LOG(LogTemp, Warning, TEXT("Tag [%s] not found! Playing Default."), *NewTag.ToString());
    }

    // 비동기 몽타주 재생
    if (!MontageToPlay.IsNull())
    {
        // 비동기 로드 로드 (헬퍼 사용)
        UEPAsyncLoadHelper::RequestAsyncLoad<UAnimMontage>(MontageToPlay,
            [this, NewTag, FoundMontageType](UAnimMontage* LoadedMontage) // 람다의 파라미터로 로드된 몽타주가 들어옴
            {
                if (!LoadedMontage) return;

                UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
                if (!AnimInstance) return;

                // 델리게이트 바인딩 (HandleMontageEnded가 UFUNCTION이어야 함)
                FOnMontageEnded EndDelegate;
                EndDelegate.BindUObject(this, &AEPCharacterBase::HandleMontageEnded, NewTag);

                // 몽타주 재생 및 상태 업데이트
                AnimInstance->Montage_Play(LoadedMontage);
                AnimInstance->Montage_SetEndDelegate(EndDelegate, LoadedMontage);

                // CombatCharacter 전용 로직 처리
                if (AEPCombatCharacterBase* CombatCharacter = Cast<AEPCombatCharacterBase>(this))
                {
                    CombatCharacter->SetCurrentInteractionMontage(LoadedMontage);
                    CombatCharacter->CurrentMontagePlay(LoadedMontage, FoundMontageType);
                }

                //CurrentActionTag = NewTag;
            }
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("tag base anim montage fide is fail"));
    }
}
