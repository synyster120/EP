// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkill_MeleeCombo.h"
#include "Components/EPSkillComponent.h"
#include "Characters/EPCombatCharacterBase.h"
#include "Data/EPSkillDataAsset.h"

void UEPSkill_MeleeCombo::Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	Super::Activate(Caster, NewTargetData, CurrentComboIndex);

    // Caster가 유효한지, SkillDataAsset이 할당되었는지 확인
    AEPCombatCharacterBase* Character = Cast<AEPCombatCharacterBase>(Caster);
    if (!Character || !SkillDataAsset) return;

    // 스킬 컴포넌트를 가져옴
    UEPSkillComponent* SkillComp = Character->GetSkillComponent();
    if (!SkillComp) return;


    const FEPSkillData& Data = SkillDataAsset->SkillData;

    // 현재 콤보 단계가 스킬 데이터에 정의된 단계 배열의 유효한 범위 내에 있는지 확인
    if (Data.ComboSequence.IsValidIndex(CurrentComboIndex))
    {
        // 현재 콤보 단계에 맞는 데이터를 가져옴
        const FEPComboStep& StepData = Data.ComboSequence[CurrentComboIndex];
        FEPSkillPhaseData SkillPhaseData;

        if (StepData.SourceType == EEPSkillPhaseSource::Local)
        {
            SkillPhaseData = Data.LocalSkillPhases[StepData.LocalPhaseIndex];
        }
        else if(StepData.SourceType == EEPSkillPhaseSource::Referenced)
        {
            if (StepData.ReferencedPhaseRow.DataTable)
            {
                // 핸들에서 직접 데이터를 찾아옴
                FEPSkillPhaseData* SkillData = StepData.ReferencedPhaseRow.GetRow<FEPSkillPhaseData>(TEXT(""));
                if (SkillData)
                {
                    SkillPhaseData = *SkillData;
                }
            }
        }

        SkillPhaseDataMontageTag = SkillPhaseData.AnimationTag;

        // 캐릭터에게 해당 태그의 애니메이션을 재생하라고 요청
        Character->PlayAnimationByTag(SkillPhaseData.AnimationTag);

        UE_LOG(LogTemp, Warning, TEXT("Skill_melee combo --> playing animation : %s"), *SkillPhaseData.AnimationTag.ToString());
    }
}
