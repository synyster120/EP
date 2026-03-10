// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkillBase.h"
#include "Data/EPSkillDataAsset.h"
#include "Data/EPSkillTypes.h"
#include "Characters/EPCharacterBase.h"

#include "Data/EPFXPreloadLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"


// 실행
void UEPSkillBase::Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	// 스킬 시전 - 자식에서 구현 
    AEPCharacterBase* OwnerCharacter = GetTypedOuter<AEPCharacterBase>();
    if (OwnerCharacter)
    {
        // 캐릭터의 몽타주 종료 델리게이트를 직접 구독
        OwnerCharacter->OnActionEnded.AddDynamic(this, &UEPSkillBase::HandleSkillEnded);
    }
}

// 몽타주 end 바인딩 함수
void UEPSkillBase::HandleSkillEnded(FGameplayTag EndedTag)
{
    //if (EndedTag.MatchesTag(FEPGameplayTags::Get().Tag_Action_Hit)) return;


    if (!EndedTag.MatchesTag(SkillPhaseDataMontageTag)) return;
    UE_LOG(LogTemp, Warning, TEXT("[TEST1] play montage : %s, skill phase data montage : %s"), *EndedTag.ToString(), *SkillPhaseDataMontageTag.ToString());
    // 몽타주 종료 감지 후 보고
    OnSkillFinishedNative.Broadcast(this);
}

void UEPSkillBase::BeginDestroy()
{
    Super::BeginDestroy();

    // 바인딩 제거
    /*AEPCharacterBase* OwnerCharacter = GetTypedOuter<AEPCharacterBase>();
    {
        OwnerCharacter->OnActionEnded.RemoveDynamic(this, &UEPSkillBase::HandleSkillEnded);
    }*/
}

// ------------------------------------------ Data Getter ------------------------------------------
inline FName UEPSkillBase::GetSkillID() const
{
    return SkillDataAsset ? FName(SkillDataAsset->SkillData.SkillName) : NAME_None;
}

FEPSkillPhaseData* UEPSkillBase::GetPhaseData(int32 CurrentPhaseDataIndex) const
{
    if (!SkillDataAsset)
    {
        return nullptr;
    }

    FEPComboStep StepData = SkillDataAsset->SkillData.ComboSequence[CurrentPhaseDataIndex];
    EEPSkillPhaseSource SkillSourceType = StepData.SourceType;

    if (SkillSourceType == EEPSkillPhaseSource::Local)
    {
        return &SkillDataAsset->SkillData.LocalSkillPhases[StepData.LocalPhaseIndex];
    }
    else if (SkillSourceType == EEPSkillPhaseSource::Referenced)
    {
        if (StepData.ReferencedPhaseRow.DataTable)
        {
            // 핸들에서 직접 데이터를 찾아옵니다.
            FEPSkillPhaseData* SkillData = StepData.ReferencedPhaseRow.GetRow<FEPSkillPhaseData>(TEXT(""));
            if (SkillData)
            {
                return SkillData;
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Skill '%s' (Combo Step %d): Referenced skill phase is selected, but the Data Table is not set!"),
                *SkillDataAsset->GetName(),
                CurrentPhaseDataIndex);
        }
    }

	return nullptr;
}

void UEPSkillBase::Initialize(UEPSkillDataAsset* NewSkillDataAsset, UObject* WorldContext)
{
    SkillDataAsset = NewSkillDataAsset;

    if (!WorldContext)
    {
        FXLib = nullptr;
        return;
    }

    UWorld* World = WorldContext->GetWorld();
    if (!World)
    {
        FXLib = nullptr;
        return;
    }

    UGameInstance* GI = World->GetGameInstance();
    if (!GI)
    {
        FXLib = nullptr;
        return;
    }

    FXLib = GI->GetSubsystem<UEPFXPreloadLibrary>();
    bool IsLoaded = true;
    if (GetPhaseData(0)->FXTag.IsValid() && !FXLib->IsLoaded(GetPhaseData(0)->FXTag)) IsLoaded = false;
    if (GetPhaseData(0)->ProjectileInfo.FXImpactTag.IsValid() && !FXLib->IsLoaded(GetPhaseData(0)->ProjectileInfo.FXImpactTag)) IsLoaded = false;
    if (GetPhaseData(0)->ProjectileInfo.FXExpireTag.IsValid() && !FXLib->IsLoaded(GetPhaseData(0)->ProjectileInfo.FXExpireTag)) IsLoaded = false;
    
    if (IsLoaded) {
        SetSkillFX();
        return;
    }

    FXLib->OnPreloadCompleted.AddDynamic(this, &UEPSkillBase::SetSkillFX);
}

void UEPSkillBase::SetSkillFX()
{
    UE_LOG(LogTemp, Warning, TEXT("Set Skill FX Name %s is Loaded"), SkillDataAsset ? *SkillDataAsset->SkillData.SkillName : TEXT("NONE"));
    const TArray<FEPComboStep>& ComboSeq = SkillDataAsset->SkillData.ComboSequence;

    for (int32 StepIdx = 0; StepIdx < ComboSeq.Num(); ++StepIdx)
    {
        FEPSkillPhaseData* Phase = GetPhaseData(StepIdx);
        if (!Phase)
        {
            continue;
        }

        const FGameplayTag& FXTag = Phase->FXTag;
        if (FXTag.IsValid()) {
            Phase->VFX = FXLib->GetVFX(FXTag);
            Phase->SFX = FXLib->GetSFX(FXTag);
        }
        const FGameplayTag& FXImTag = Phase->ProjectileInfo.FXImpactTag;
        if (FXImTag.IsValid()) {
            Phase->ProjectileInfo.ImpactEffect = FXLib->GetVFX(FXImTag);
            Phase->ProjectileInfo.ImpactSound = FXLib->GetSFX(FXImTag);
        }
        const FGameplayTag& FXExTag = Phase->ProjectileInfo.FXExpireTag;
        if (FXExTag.IsValid()) {
            Phase->ProjectileInfo.ExpireEffect = FXLib->GetVFX(FXExTag);
            Phase->ProjectileInfo.ExpireSound = FXLib->GetSFX(FXExTag);
        }
    }
}

bool UEPSkillBase::RequiresMovementLock(int32 CurrentPhaseDataIndex) const
{
    if (FEPSkillPhaseData* data = GetPhaseData(CurrentPhaseDataIndex))
    {
        return data->bRequiresMovementLock;
    }
    return false;
}

float UEPSkillBase::GetWindupSeconds(int32 CurrentPhaseDataIndex) const
{
    if (FEPSkillPhaseData* data = GetPhaseData(CurrentPhaseDataIndex))
    {
        return data->WindupSeconds;
    }
    return 0.0f;
}
