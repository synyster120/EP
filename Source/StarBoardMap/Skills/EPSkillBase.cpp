// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkillBase.h"
#include "Data/EPSkillDataAsset.h"
#include "Data/EPSkillTypes.h"

void UEPSkillBase::Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	// 스킬 시전 - 자식에서 구현
    
}

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

void UEPSkillBase::Initialize(UEPSkillDataAsset* NewSkillDataAsset)
{
    SkillDataAsset = NewSkillDataAsset;
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