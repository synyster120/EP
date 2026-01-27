
#include "Components/EPSkillComponent.h"
#include "Data/EPSkillDataAsset.h"
#include "Components/EPStatComponent.h"
#include "Core/Helper/EPAsyncLoadHelper.h"
#include "Skills/EPSkillBase.h"
#include "GameFramework/Character.h"
#include "Characters/EPCharacterBase.h"
#include "Characters/EPEnemyCharacter.h"
#include "Skills/Targeting/EPTargetingStrategy.h"
#include "Components/EPMovementLockComponent.h"

#include "Skills/Targeting/EPTargetingStrategy.h"

UEPSkillComponent::UEPSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FEPSkillRangeData UEPSkillComponent::ReturnSkillRangeData()
{
    return SkillSlots[LastkillSlotIndex].SkillObject->GetPhaseData(LastComboSkillIndex)->SkillRange;
}

float UEPSkillComponent::ReturnDamage()
{
    return SkillSlots[LastkillSlotIndex].SkillObject->GetPhaseData(LastComboSkillIndex)->Damage;
}

void UEPSkillComponent::BeginPlay()
{
    Super::BeginPlay();

    // 이 컴포넌트의 소유자(캐릭터)를 찾아 StatComponent를 가져옴
    //AActor* Owner = GetOwner();
    //if (Owner)
    //{
    //    StatComponentRef = Owner->FindComponentByClass<UEPStatComponent>();
    //}

    //// StatComponent는 반드시 존재해야 하므로, 없다면 에러를 발생시켜 문제를 즉시 인지하게 함
    //check(StatComponentRef != nullptr);

    // target 지정 방식 객체 생성
    /*if (DefaultStrategyClass)
    {
        CachedStrategy = NewObject<UEPTargetingStrategy>(this, DefaultStrategyClass);
    }*/
}

// 초기화 함수 (외부에서 호출)
void UEPSkillComponent::InitializeSkills(const TArray<TSoftObjectPtr<UEPSkillDataAsset>>& SkillAssets)
{
    // 스킬을 새로 생성하기 전에 기존 상태를 깨끗하게 정리
    ClearSkills();

    // 데이터 테이블로 스킬 목록 초기화
    CreateSkills(SkillAssets);
}

void UEPSkillComponent::ClearSkills()
{
    // 월드와 타이머 매니저가 유효한지 확인
    UWorld* World = GetWorld();
    if (World)
    {
        FTimerManager& TimerManager = World->GetTimerManager();
        // 실행 중인 모든 쿨타임 타이머를 중지
        for (const auto& Elem : SkillIDToIndexMap)
        {
            if (SkillSlots.IsValidIndex(Elem.Value))
            {
                FTimerHandle& TimerHandle = SkillSlots[Elem.Value].CooldownTimerHandle;
                if (TimerHandle.IsValid())
                {
                    TimerManager.ClearTimer(TimerHandle);
                }
            }
        }
    }

    // 스킬 객체는 UObject이므로 별도로 파괴할 필요 없이 참조만 끊어주면
    // 가비지 컬렉터(GC)가 알아서 메모리를 정리해줌
    SkillSlots.Empty();
    SkillIDToIndexMap.Empty();
}

// 노티파이에 의해 bCanCombo가 true가 될 때 호출되는 함수 (Setter)
void UEPSkillComponent::SetbIsCancelWindowActive(bool bInIsCancelWindowActive)
{
    bIsCancelWindowActive = bInIsCancelWindowActive;

    // 구간이 열리는 순간(true), 예약된 입력(Buffer)이 있다면 실행
    if (bIsCancelWindowActive && bInputBuffer)
    {
        if (BufferSkillIndex >= 0)
        {
            ActivateSkill(BufferSkillIndex);
            ClearBuffer(); // 실행 후 버퍼 비우기
        }
    }
}

// Buffer 관련 변수 초기화
void UEPSkillComponent::ClearBuffer()
{
    bInputBuffer = false;
    BufferSkillIndex = -1;
}

// 스킬 시전하기 전 input 상태 확인 후 시전 시작 (외부에서 호출)
void UEPSkillComponent::ProcessSkillInput(int32 SkillIndex)
{
    // 상태가 있는 character인지 확인
    AEPCharacterBase* OwnerCaster = Cast<AEPCharacterBase>(GetOwner());
    if (OwnerCaster)
    {
        EEPCharacterState OwnerCurrentState = OwnerCaster->GetCurrentState();
        // 현재 스킬이 실행 중이 아니라면 (Idle) 바로 실행
        if (OwnerCurrentState == EEPCharacterState::Idle)
        {
            // 스킬을 사용하기 직전에, 캐릭터의 상태를 'Attacking'으로 변경
            OwnerCaster->SetCurrentState(EEPCharacterState::Attacking);

            UE_LOG(LogTemp, Warning, TEXT("player is attacking and idle state --> activate() play"));
            ActivateSkill(SkillIndex);

            return;
        }

        // 이미 스킬 실행 중일 때 (공격 중일 때)
        if (OwnerCurrentState == EEPCharacterState::Attacking)
        {
            UE_LOG(LogTemp, Warning, TEXT("player is attacking and Attacking state --> bIsCancelWindowActive check"));
            if (bIsCancelWindowActive) // 다음 스킬로 넘어갈 수 있는 구간인지 확인
            {
                ActivateSkill(SkillIndex);
            }
            else // 넘어갈 수 있는 구간은 아니지만 공격 중이므로 "예약(Buffer)" 함
            {
                bInputBuffer = true;
                BufferSkillIndex = SkillIndex; // 어떤 스킬을 예약했는지 저장
                UE_LOG(LogTemp, Log, TEXT("Skill Buffered!"));
            }
        }
    }
    else // character가 아닐 경우, 그냥 skill 시전
    {
        ActivateSkill(SkillIndex);
    }

}

// 스킬 슬롯 생성 함수 (내부에서 호출)
void UEPSkillComponent::CreateSkills(const TArray<TSoftObjectPtr<UEPSkillDataAsset>>& SkillAssets)
{
    // 기존 스킬 슬롯 초기화
    SkillSlots.Empty();
    SkillIDToIndexMap.Empty();

    // SkillAssets의 개수만큼 SkillSlots 배열의 크기 할당
    SkillSlots.SetNum(SkillAssets.Num());

    for (int32 Index = 0; Index < SkillAssets.Num(); Index++)
    {
        const TSoftObjectPtr<UEPSkillDataAsset>& SkillAssetPtr = SkillAssets[Index];

        // 스킬 데이터 애셋 로드 요청
        UEPAsyncLoadHelper::RequestAsyncLoad<UEPSkillDataAsset>(SkillAssetPtr,
            [this, Index](UEPSkillDataAsset* LoadedSkillDataAsset)
            {
                // 스킬 데이터 애셋 로드 정상 완료 확인
                if (!LoadedSkillDataAsset)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to load SkillDataAsset at index %d"), Index);
                    return;
                }

                const FEPSkillData& SkillData = LoadedSkillDataAsset->SkillData;

                // 스킬 클래스 비동기 로드 요청
                UEPAsyncLoadHelper::RequestAsyncLoad<UEPSkillBase>(SkillData.SkillClass,
                    [this, Index, SkillData, LoadedSkillDataAsset](TSubclassOf<UEPSkillBase> LoadedSkillClass)
                    {
                        // 스킬 클래스 로드 정상 완료 확인
                        if (!LoadedSkillClass)
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Failed to load SkillClass for %s"), *SkillData.SkillName);
                            return;
                        }

                        // 스킬 객체 생성 (UEPSkillBase는 추상화 클래스라 타입에 넣어서 생성 못함) + NewObject: 생성 객체 타입을 컴파일 시점에 결정
                        UEPSkillBase* NewSkill = NewObject<UEPSkillBase>(GetOwner(), LoadedSkillClass);

                        if (NewSkill)
                        {
                            // 스킬 객체 초기화
                            NewSkill->Initialize(LoadedSkillDataAsset, GetOwner());
                            // 스킬슬롯에 객체 저장
                            SkillSlots[Index].SkillObject = NewSkill;
                            // 스킬슬롯에 최대 콤보 저장
                            int32 MaxCombo = SkillData.ComboSequence.Num();
                            SkillSlots[Index].MaxComboCount = MaxCombo;

                            // 스킬 검색 Map에 스킬 정보 추가
                            FName SkillID = FName(SkillData.SkillName);
                            SkillIDToIndexMap.Add(SkillID, Index);
                            
                            UE_LOG(LogTemp, Log, TEXT("[ %s ] Skill '%s' created at index %d"), *GetOwner()->GetName(), *SkillID.ToString(), Index);
                        }
                    });
            });

    }
}

// 시킬 시전 함수 (내부에서 호출)
void UEPSkillComponent::ActivateSkill(int32 SkillIndex)
{
    // 유효성 검사 (인덱스, 쿨타임 등)
    if (!CanActivateSkill(SkillIndex)) return;

    UEPSkillBase* SkillToActivate = SkillSlots[SkillIndex].SkillObject;
    if (SkillToActivate == nullptr) return;

    // 콤보 상태 결정 (가장 핵심적인 로직)
    FSkillRuntimeData& SkillSlot = SkillSlots[SkillIndex];

    // 콤보 스킬인지 확인
    if (SkillSlot.MaxComboCount > 1)
    {
        int32& ComboCounter = ComboStateMap.FindOrAdd(SkillIndex); // 현재 실행할 콤보 단계 검색 및 가져오기 (없으면 생성됨)
        if (LastkillSlotIndex == SkillIndex && bIsCancelWindowActive) // && GetWorld()->GetTimerManager().IsTimerActive(ComboTimerHandle)) // 동일한 스킬 슬롯인지 And 타이머가 작동중인지 확인
        {
            LastComboSkillIndex++; // 다음 콤보로
        }
        else
        {
            LastComboSkillIndex = 0; // 콤보 초기화 (1타)
        }

        // 콤보 순환
        if (LastComboSkillIndex >= SkillSlot.MaxComboCount)
        {
            // 콤보 리셋
            LastComboSkillIndex = 0;
        }
        ComboCounter = LastComboSkillIndex; // 최종 콤보 인덱스 결정

        UE_LOG(LogTemp, Warning, TEXT("cobo state | combo num : %d"), LastComboSkillIndex);
    }
    // 스킬 실행   
    ActivateSkillFinished(SkillIndex);

    // 실행했으므로 다시 구간이 열리기 전까지는 false
    bIsCancelWindowActive = false;
}

// 실제 스킬 실행 처리
void UEPSkillComponent::ActivateSkillFinished(int32 SkillIndex)
{
    OnMovementLockEnded.Broadcast();

    UEPSkillBase* SkillToActivate = SkillSlots[SkillIndex].SkillObject;
    if (SkillToActivate == nullptr) return;

    UE_LOG(LogTemp, Warning, TEXT("[ %s ] ActivateSkill | skill index : %d"), *GetOwner()->GetName(), SkillIndex);


    // 스킬 객체에 '실행' 명령
    if (SkillToActivate)
    {
        LastkillSlotIndex = SkillIndex; // 마지막에 사용한 스킬 슬롯 저장

        // 타겟팅 로직을 통해 TargetData 생성
        FEPSkillTargetData TargetData;
        if (PerformTargeting(SkillToActivate, LastComboSkillIndex, TargetData))
        {
            // 스킬 사용 (최종)
            AEPCharacterBase* OwnerCaster = Cast<AEPCharacterBase>(GetOwner());
            if (OwnerCaster)
            {
                // 스킬을 사용하기 직전에, 캐릭터의 상태를 'Attacking'으로 변경
                OwnerCaster->SetCurrentState(EEPCharacterState::Attacking);
            }

            // 스킬 실행 (스킬 객체에 요청)
            SkillToActivate->Activate(GetOwner(), TargetData, LastComboSkillIndex);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Skill [%s] PerformTargeting is fail."), *SkillToActivate->GetSkillID().ToString());
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Skill [%s] object is null ptr."), *GetName());
    }

    // 콤보 쿨타임 설정 및 스킬 쿨타임 설정
    StartComboWindow(SkillIndex, LastComboSkillIndex);
    //StartCooldown(SkillIndex); // index로 쿨타임 시작 
    StartCooldown(SkillToActivate->GetSkillID());
}

// 스킬 사용 가능 여부 판단
bool UEPSkillComponent::CanActivateSkill(int32 SkillIndex)
{
    // 유효한 스킬 인덱스인가?
    if (!SkillSlots.IsValidIndex(SkillIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Skill [%s] index is null."), *SkillSlots[SkillIndex].SkillObject->GetSkillID().ToString());
        return false;
    }

    // 해당 스킬이 쿨타임 중인가?
    if (SkillSlots[SkillIndex].CooldownTimerHandle.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Skill [%s] is on cooldown."), *SkillSlots[SkillIndex].SkillObject->GetSkillID().ToString());
        return false;
    }
    else 
    {

        UE_LOG(LogTemp, Warning, TEXT("CanActivateSkill | skill index : %d"), SkillIndex);
    }

    // 마나가 충분한가?
    // if (StatComponent->GetCurrentMana() < RequiredMana) return false;

    // 플레이어가 침묵 상태가 아닌가?
    // if (StatComponent->HasState(EState::Silenced)) return false;

    // 스킬 사용 가능
    return true;
}

// 스킬 쿨타임 시작
void UEPSkillComponent::StartCooldown(FName SkillID)
{
    UE_LOG(LogTemp, Warning, TEXT("StartCooldown | skill id : %s"), *SkillID.ToString());
    // 맵을 사용해 O(1) 시간 복잡도로 인덱스를 즉시 찾음
    if (const int32* IndexPtr = SkillIDToIndexMap.Find(SkillID))
    {
        FSkillRuntimeData& SkillData = SkillSlots[*IndexPtr];

        //스킬 데이터에서 쿨타임 정보를 가져옴
        const float CooldownDuration = SkillData.SkillObject->GetSkillData()->SkillData.Cooldown;
        if (CooldownDuration >= 0.0f)
        {
            //if (Cast<AEPEnemyCharacter>(GetOwner())) // enmey 일 경우, 스킬 무한 지속
            //{
            //    UE_LOG(LogTemp, Warning, TEXT("Cast<AEPEnemyCharacter>(GetOwner()) is true --> enemy timer play :: 쿨타임 : %f"), CooldownDuration);
            //    GetWorld()->GetTimerManager().SetTimer(SkillData.CooldownTimerHandle, [this, IndexPtr, &SkillData]()
            //        {
            //            UE_LOG(LogTemp, Warning, TEXT("enemy timer end --> activateSkill() play"));
            //            GetWorld()->GetTimerManager().ClearTimer(SkillData.CooldownTimerHandle);
            //            ActivateSkill(*IndexPtr);
            //        }, CooldownDuration, false);
            //}
            //else // 기본, 스킬 1번 쿨타임
            {
                //UE_LOG(LogTemp, Warning, TEXT("Cast<AEPEnemyCharacter>(GetOwner()) is false --> player timer play :: skill index : %d "), *IndexPtr);
                FTimerHandle NewTimerHandle;
                // 델리게이트를 사용하여 쿨타임이 끝나면 OnCooldownFinished 함수가 호출되도록 설정
                FTimerDelegate CooldownDelegate = FTimerDelegate::CreateUObject(this, &UEPSkillComponent::OnCooldownFinished, SkillID);

                // 스킬 쿨 타이머 세팅 및 스킬 슬롯에 쿨타임 설정
                GetWorld()->GetTimerManager().SetTimer(SkillData.CooldownTimerHandle, CooldownDelegate, CooldownDuration, false);
            }
        }
    }
}

// 스킬 쿨타임 종료 = 스킬 사용 가능 세팅
void UEPSkillComponent::OnCooldownFinished(FName SkillID)
{
    // 쿨타임이 종료되었으므로 맵에서 해당 스킬을 제거

    if (const int32* IndexPtr = SkillIDToIndexMap.Find(SkillID))
    {
        FSkillRuntimeData& SkillData = SkillSlots[*IndexPtr];

        // 타이머를 완전히 정리하고 핸들 무효화
        GetWorld()->GetTimerManager().ClearTimer(SkillData.CooldownTimerHandle);
        UE_LOG(LogTemp, Log, TEXT("Skill [%s] cooldown finished."), *SkillID.ToString());

        // task 바인딩
        OnSkillCooldownEnded.Broadcast(*IndexPtr, GetOwner());
    }
}

// 콤보 타이머 시작
void UEPSkillComponent::StartComboWindow(int32 SkillIndex, int32 CurrentComboIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("StartComboWindow | skill index : %d"), SkillIndex);
    // 스킬 객체에서 현재 콤보 단계의 유효시간 데이터를 가져옴
    const float ComboWindow = SkillSlots[SkillIndex].SkillObject->GetPhaseData(CurrentComboIndex)->ComboValidTime;

    // 데이터에 콤보 유효시간이 설정되어 있을 때만 타이머를 돌림
    if (ComboWindow > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UEPSkillComponent::ResetCombo, ComboWindow, false);
    }
}

// 콤보 타이머 초기화
void UEPSkillComponent::ResetCombo()
{
    UE_LOG(LogTemp, Log, TEXT("reset combo"));
    // 콤보 유효시간이 지나면, 마지막 콤보 기록 초기화
    LastComboSkillIndex = 0;
}

// 스킬 단계의 필요한 타겟 타입 맞춰서 타겟 지정
bool UEPSkillComponent::PerformTargeting(UEPSkillBase* SkillToActivate, int32 SkillIndex, FEPSkillTargetData& OutTargetData)
{
    // 실행
    if (TargetingStrategy)
    {
        AActor* Owner = GetOwner();

        // 해당 전략에 따라 타겟을 찾도록 '위임'
        const FEPSkillPhaseData* PhaseData = SkillSlots[SkillIndex].SkillObject->GetPhaseData(LastComboSkillIndex);
        if (!PhaseData) return false;

        if (TargetingStrategy->FindTarget(Owner, *PhaseData, OutTargetData))
        {
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[%s] PerformTargeting() -> FindTarget() is fail"), *GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] character - TargetingStrategyClass setting is null -> FindTarget() is fail"), *GetName());
    }

    return false;
}

