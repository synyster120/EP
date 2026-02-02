// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkillBase.h"
#include "Data/EPSkillDataAsset.h"
#include "Data/EPSkillTypes.h"
#include "Characters/EPCharacterBase.h"

// 초기화
void UEPSkillBase::Initialize(UEPSkillDataAsset* NewSkillDataAsset)
{
    SkillDataAsset = NewSkillDataAsset;

    AEPCharacterBase* OwnerCharacter = GetTypedOuter<AEPCharacterBase>();
    if (OwnerCharacter)
    {
        // 캐릭터의 몽타주 종료 델리게이트를 직접 구독
        OwnerCharacter->OnActionEnded.AddDynamic(this, &UEPSkillBase::HandleSkillEnded);
    }
}

// 실행
void UEPSkillBase::Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	// 스킬 시전 - 자식에서 구현   
}

bool UEPSkillBase::RequiresMovementLock(int32 CurrentPhaseDataIndex) const
{
    if (FEPSkillPhaseData* data = GetPhaseData(CurrentPhaseDataIndex))
    {
        return data->bRequiresMovementLock;
    }
    return false;
}

// 몽타주 end 바인딩 함수
void UEPSkillBase::HandleSkillEnded(FGameplayTag EndedTag)
{
    // "공격중" 상태 리셋 
    //AEPCharacterBase* OwnerCharacter = GetTypedOuter<AEPCharacterBase>();
    //if (OwnerCharacter)
    //{
    //    // 이걸 여기서 하는 게 맞나? skillcomponent의 관리 역할이 아닌가?
    //}
    
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

float UEPSkillBase::GetWindupSeconds(int32 CurrentPhaseDataIndex) const
{
    if (FEPSkillPhaseData* data = GetPhaseData(CurrentPhaseDataIndex))
    {
        return data->WindupSeconds;
    }
    return 0.0f;
}
