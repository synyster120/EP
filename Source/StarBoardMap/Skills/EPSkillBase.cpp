// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkillBase.h"
#include "Data/EPSkillDataAsset.h"


void UEPSkillBase::Activate(ACharacter* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	// 스킬 시전
	UE_LOG(LogTemp, Warning, TEXT("SkillBase Activate on"));

}

inline FName UEPSkillBase::GetSkillID() const
{
	return SkillDataAsset ? FName(SkillDataAsset->SkillData.SkillName) : NAME_None;
}

FEPSkillPhaseData UEPSkillBase::GetPhaseData(int32 CurrentPhaseDataIndex)
{
	return FEPSkillPhaseData();
}

void UEPSkillBase::Initialize(UEPSkillDataAsset* NewSkillDataAsset)
{
    SkillDataAsset = NewSkillDataAsset;
}
