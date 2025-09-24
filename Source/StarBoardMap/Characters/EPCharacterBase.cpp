
#include "Characters/EPCharacterBase.h"
#include "Core/Helper/EPAsyncLoadHelper.h"
#include "Data/EPCharacterAnimationData.h"


AEPCharacterBase::AEPCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false;

}

// 애니메이션 검색 후 Play 
void AEPCharacterBase::PlayAnimationByTag(FGameplayTag NewTag)
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
            [this](UAnimMontage* LoadedMontage) // 람다의 파라미터로 로드된 몽타주가 들어옴
            {
                UE_LOG(LogTemp, Warning, TEXT("anim helper in"));
                if (LoadedMontage)
                {
                    UE_LOG(LogTemp, Warning, TEXT("anim helper in playing animmontage"));
                    PlayAnimMontage(LoadedMontage);
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
