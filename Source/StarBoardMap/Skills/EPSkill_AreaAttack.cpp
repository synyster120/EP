// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkill_AreaAttack.h"
#include "Characters/EPCombatCharacterBase.h"
#include "Data/EPSkillDataAsset.h"
#include "Components/EPMovementLockComponent.h"
#include "Core/EPGameplayTags.h" // Tag

void UEPSkill_AreaAttack::Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{

    // Caster가 유효한지, SkillDataAsset이 할당되었는지 확인
    AEPCombatCharacterBase* Character = Cast<AEPCombatCharacterBase>(Caster);
    if (!Character || !SkillDataAsset) return;

    const FEPSkillData& Data = SkillDataAsset->SkillData;
    // 현재 콤보 단계가 스킬 데이터에 정의된 단계 배열의 유효한 범위 내에 있는지 확인
    if (Data.ComboSequence.IsValidIndex(CurrentComboIndex))
    {
        // 현재 콤보 단계에 맞는 데이터를 가져옴
        const FEPComboStep& StepData = Data.ComboSequence[CurrentComboIndex];
        
        if (FEPSkillPhaseData* SkillPhaseData = GetPhaseData(CurrentComboIndex))
        {
            // 몽타주 종료 바인딩
            FOnMontageEnded OnMontageEndedDelegate;
            OnMontageEndedDelegate.BindUObject(this, &UEPSkill_AreaAttack::OnSkillMontageEnded, SkillPhaseData->AnimationTag);

            // [로그 추가] 어떤 인스턴스(this)가 바인딩을 시도하는지 기록
            UE_LOG(LogTemp, Warning, TEXT("Skill [0x%p] BINDING delegate to MontageEnd"), this);

            SkillPhaseDataMontageTag = SkillPhaseData->AnimationTag;
            // 캐릭터에게 해당 태그의 애니메이션을 재생하라고 요청
            Character->PlayAnimationByTag(SkillPhaseData->AnimationTag, OnMontageEndedDelegate);

            // 움직임 제어 (Lock)
            MovementLock = Character->GetMovementLockComponent();
            if (MovementLock)
            {
                // Lock 설정
                MovementLock->Acquire(Locktext);
            }

            UE_LOG(LogTemp, Warning, TEXT("Skill_AreaAttack --> playing animation : %s"), *SkillPhaseData->AnimationTag.ToString());

        }
    }
}

void UEPSkill_AreaAttack::OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGameplayTag ActionTag)
{
    UE_LOG(LogTemp, Warning, TEXT("start lock relesase"));
    // 이동 잠금 해제
    if (MovementLock && ActionTag.MatchesTag(FEPGameplayTags::Get().Tag_Skill_Attack))
    {
        MovementLock->Release(Locktext);
    }
}

// 이 객체가 삭제될 때
void UEPSkill_AreaAttack::BeginDestroy()
{
    Super::BeginDestroy();
    //       (주목도를 높이기 위해 Error 레벨로 출력)
    UE_LOG(LogTemp, Error, TEXT("Skill [0x%p] BEING DESTROYED (GC'd)"), this);
    UE_LOG(LogTemp, Warning, TEXT("[%s] skill object distroy"), *this->GetName());
}