
#include "Characters/EPCharacterBase.h"
#include "Core/Helper/EPAsyncLoadHelper.h"
#include "Data/EPCharacterAnimationData.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Characters/EPCombatCharacterBase.h"
#include "Components/EPMovementLockComponent.h"


AEPCharacterBase::AEPCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false;
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);

    MovementLockComponent = CreateDefaultSubobject<UEPMovementLockComponent>(TEXT("MovementLockComponent"));
}

// 애니메이션 검색 후 Play 
void AEPCharacterBase::PlayAnimationByTag(FGameplayTag NewTag, const FOnMontageEnded& OnMontageEndedDelegate)
{
	// 태그에 맞는 몽타주 검색 후 Play
    if (!AnimDataAsset) return;

    // TMap에서 태그에 맞는 몽타주 포인터를 찾습니다.
    TSoftObjectPtr<UAnimMontage>* FoundMontagePtr = AnimDataAsset->SkillAnimationMontage.Find(NewTag);

    if (!FoundMontagePtr)
    {
        FoundMontagePtr = AnimDataAsset->InteractionMontages.Find(NewTag);
    }

    if (FoundMontagePtr)
    {
        // 비동기 로드 로드 (헬퍼 사용)
        UEPAsyncLoadHelper::RequestAsyncLoad<UAnimMontage>(*FoundMontagePtr,
            [this, OnMontageEndedDelegate] (UAnimMontage* LoadedMontage) // 람다의 파라미터로 로드된 몽타주가 들어옴
            {
                if (LoadedMontage)
                {
                    AEPCombatCharacterBase* CombatCharacter = Cast<AEPCombatCharacterBase>(this);
                    if (CombatCharacter)
                    {
                        // ### 핵심 로직 ###
                        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
                        if (AnimInstance)
                        {
                            // const를 non-const로 사용하기 위해 복사
                            FOnMontageEnded NonConstDelegateCopy = OnMontageEndedDelegate;
                            //UE_LOG(LogTemp, Warning, TEXT("anim helper in playing animmontage"));

                            // 몽타주 재생
                            AnimInstance->Montage_Play(LoadedMontage);
                            CombatCharacter->SetCurrentInteractionMontage(LoadedMontage); // 몽타주 저장


                            // "몽타주를 재생 후" 스킬로부터 넘겨받은 델리게이트 등록
                            AnimInstance->Montage_SetEndDelegate(NonConstDelegateCopy, LoadedMontage);
                        }
                        // ### ###


                        if (CurrentState == EEPCharacterState::Attacking)
                        {
                            CombatCharacter->CurrentMontagePlay(LoadedMontage, EEPCombatMontageType::Attack);
                        }
                        else
                        {
                            CombatCharacter->CurrentMontagePlay(LoadedMontage, EEPCombatMontageType::None);
                        }
                    }
                    else
                    {
                        PlayAnimMontage(LoadedMontage);
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("anim data asset -> AnimationMontage is not set in % s!"), *AnimDataAsset->GetName());
                }
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
