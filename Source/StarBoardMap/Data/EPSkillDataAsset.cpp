// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/EPSkillDataAsset.h"

// CreateSkill 에서 이미 로드된 생태. 가져와서 사용만 함
const bool UEPSkillDataAsset::GetProjectileData_Implementation(int32 PhaseIndex, FEPProjectileData& OutProjectileData) const
{
    // 자신의 멤버 변수인 SkillData에서 ComboSequence 배열 가져옴
    if (SkillData.ComboSequence.IsValidIndex(PhaseIndex))
    {
        const FEPComboStep& StepData = SkillData.ComboSequence[PhaseIndex];

        const FEPSkillPhaseData* PhaseDataPtr = nullptr;

        // 콤보 단계의 소스 타입 확인
        if (StepData.SourceType == EEPSkillPhaseSource::Local)
        {
            if (SkillData.LocalSkillPhases.IsValidIndex(StepData.LocalPhaseIndex))
            {
                PhaseDataPtr = &SkillData.LocalSkillPhases[StepData.LocalPhaseIndex];
            }
        }
        else if (StepData.SourceType == EEPSkillPhaseSource::Referenced)
        {
            if (StepData.ReferencedPhaseRow.DataTable)
            {
                PhaseDataPtr = StepData.ReferencedPhaseRow.GetRow<FEPSkillPhaseData>("");
            }
        }

        // 유효한 스킬 단계 데이터를 찾았다면, 투사체 정보 복사 후 성공을 반환
        if (PhaseDataPtr)
        {
            OutProjectileData = PhaseDataPtr->ProjectileInfo;
            return true; // 성공 반환
        }
    }

    // 실패 반환
    return false;
}
