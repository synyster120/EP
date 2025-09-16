#include "Components/EPSkillComponent.h"
#include "Data/EPSkillDataAsset.h"
#include "Skills/EPSkillBase.h"
#include "Components/EPStatComponent.h"

UEPSkillComponent::UEPSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEPSkillComponent::BeginPlay()
{
    Super::BeginPlay();

    // 이 컴포넌트의 소유자(캐릭터)를 찾아 StatComponent를 가져옴
    AActor* Owner = GetOwner();
    if (Owner)
    {
        StatComponentRef = Owner->FindComponentByClass<UEPStatComponent>();
    }

    // StatComponent는 반드시 존재해야 하므로, 없다면 에러를 발생시켜 문제를 즉시 인지하게 함
    check(StatComponentRef != nullptr);
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

// 스킬 슬롯 생성 함수 (내부에서 호출)
void UEPSkillComponent::CreateSkills(const TArray<TSoftObjectPtr<UEPSkillDataAsset>>& SkillAssets)
{
    // 기존 스킬 슬롯 초기화
    SkillSlots.Empty();
    SkillIDToIndexMap.Empty();

    // SkillAssets의 개수만큼 SkillSlots 배열의 크기 할당
    SkillSlots.SetNum(SkillAssets.Num());

    // 배열 수
    int32 NewSkillIndex = SkillAssets.Num() - 1;

    for (int32 Index = 0; Index < SkillAssets.Num(); ++Index)
    {
        const TSoftObjectPtr<UEPSkillDataAsset>& SkillAssetPtr = SkillAssets[Index];

        // 소프트 포인터로부터 실제 데이터 에셋을 로드
        UEPSkillDataAsset* SkillDataAsset = SkillAssetPtr.LoadSynchronous();
        if (SkillDataAsset == nullptr) continue;

        // 데이터 에셋에서 실제 스폰할 스킬 클래스 정보를 가져옴
        TSubclassOf<UEPSkillBase> SkillClass = SkillDataAsset->SkillData.SkillClass.LoadSynchronous();
        if (SkillClass == nullptr) continue;

        // 스킬 객체 생성. 소유자는 이 컴포넌트의 소유자(캐릭터)로 지정
        UEPSkillBase* NewSkill = NewObject<UEPSkillBase>(GetOwner(), SkillClass);
        if (NewSkill)
        {
            // 생성된 스킬 객체에 필요한 데이터를 전달하여 초기화
            NewSkill->Initialize(SkillDataAsset);

            // 올바른 인덱스에 스킬 객체를 할당
            SkillSlots[NewSkillIndex].SkillObject = NewSkill;

            // 스킬 Index관리Map 에 스킬 정보 추가
            FName SkillID = NewSkill->GetSkillID();
            SkillIDToIndexMap.Add(SkillID, NewSkillIndex);
        }
    }
}

// 시킬 시전 함수 (외부에서 호출)
void UEPSkillComponent::ActivateSkill(int32 SkillIndex, const FEPSkillTargetData& TargetData)
{
    // 유효한 스킬 인덱스인지 확인
    if (!SkillSlots.IsValidIndex(SkillIndex)) return;

    UEPSkillBase* SkillToActivate = SkillSlots[SkillIndex].SkillObject;
    if (SkillToActivate == nullptr) return;

    // 쿨타임 중인지 확인
    //if (CooldownTimers.Contains(SkillToActivate->GetSkillID()))
    if (SkillSlots[SkillIndex].CooldownTimerHandle.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Skill [%s] is on cooldown."), *SkillToActivate->GetSkillID().ToString());
        return;
    }

    // 스킬을 발동하고, 쿨타임을 시작
    SkillToActivate->Activate(TargetData);
    StartCooldown(SkillToActivate->GetSkillID());
}

void UEPSkillComponent::StartCooldown(FName SkillID)
{
    // 맵을 사용해 O(1) 시간 복잡도로 인덱스를 즉시 찾음
    if (const int32* IndexPtr = SkillIDToIndexMap.Find(SkillID))
    {
        FSkillRuntimeData& SkillData = SkillSlots[*IndexPtr];

        // 이제 SkillData.CooldownTimerHandle을 사용해 타이머를 설정...

        // 스킬 데이터에서 쿨타임 정보를 가져옴
        const float CooldownDuration = SkillData.SkillObject->GetSkillData()->SkillData.Cooldown;
        if (CooldownDuration >= 0.0f)
        {
            FTimerHandle NewTimerHandle;
            // 델리게이트를 사용하여 쿨타임이 끝나면 OnCooldownFinished 함수가 호출되도록 설정
            FTimerDelegate CooldownDelegate = FTimerDelegate::CreateUObject(this, &UEPSkillComponent::OnCooldownFinished, SkillID);
            
            // 스킬 쿨 타이머 세팅 및 스킬 슬롯에 쿨타임 설정
            GetWorld()->GetTimerManager().SetTimer(SkillData.CooldownTimerHandle, CooldownDelegate, CooldownDuration, false);
        }
        else if (CooldownDuration == -1) // 스킬 무한 지속
        {
            // 로직 추가 or 빈로직
        }
    }
}

void UEPSkillComponent::OnCooldownFinished(FName SkillID)
{
    // 쿨타임이 종료되었으므로 맵에서 해당 스킬을 제거

    if (const int32* IndexPtr = SkillIDToIndexMap.Find(SkillID))
    {
        FSkillRuntimeData& SkillData = SkillSlots[*IndexPtr];

        // 타이머를 완전히 정리하고 핸들 무효화
        GetWorld()->GetTimerManager().ClearTimer(SkillData.CooldownTimerHandle);
        UE_LOG(LogTemp, Log, TEXT("Skill [%s] cooldown finished."), *SkillID.ToString());
    }
}

