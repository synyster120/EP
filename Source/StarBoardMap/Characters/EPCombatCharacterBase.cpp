// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EPCombatCharacterBase.h"
#include "Components/EPStatComponent.h"
#include "Components/EPSkillComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/EPCharacterTypes.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
// 테스트용
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Characters/EPPlayerCharacter.h"

AEPCombatCharacterBase::AEPCombatCharacterBase()
{
    // 메쉬(SkeletalMeshComponent) 설정
    // 캡슐의 Z축 아래로 90도 내리고, 캡슐을 기준으로 90도 회전
    GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -90.0));
    GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

    SkillComponent = CreateDefaultSubobject<UEPSkillComponent>(TEXT("SkillComponent"));
}

void AEPCombatCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    // BeginPlay는 게임이 시작될 때 호출됩니다.
    // 여기서 캐릭터의 데이터를 초기화하는 함수를 호출하는 것이 일반적
    InitializeCharacterData();

    if (StatComponent)
    {
        // 죽음, 피격(애니메이션) 바인딩
        StatComponent->OnDied.AddDynamic(this, &AEPCombatCharacterBase::HandleDeath_Implementation);
        StatComponent->OnHitReact.AddDynamic(this, &AEPCombatCharacterBase::HandleHitReaction);
    }
    
    // ===================== TakeDamage 테스트용 ===================================================================================
    // 가짜(Mock) 데미지 값
    float MockDamageAmount = 20.0f;

    // 가짜 FDamageEvent (가장 간단한 FPointDamageEvent로 생성)
    FPointDamageEvent MockDamageEvent;
    MockDamageEvent.HitInfo.ImpactPoint = GetActorLocation(); // 피격 위치
    MockDamageEvent.ShotDirection = GetActorForwardVector();   // 피격 방향

    // 가짜 가해자 정보 (여기서는 자기 자신으로 설정)
    AController* MockInstigator = GetController();
    AActor* MockDamageCauser = this;

    // --- 2. SetTimer와 람다를 사용하여 3초 후에 TakeDamage를 호출합니다. ---
    FTimerHandle TestTimerHandle;
    float Delay = 3.0f; // 3초 후에 실행

    GetWorld()->GetTimerManager().SetTimer(
        TestTimerHandle,
        [this, MockDamageAmount, MockDamageEvent, MockInstigator, MockDamageCauser]() // 람다 캡처
        {
            // 람다 내부에서는 캡처한 변수가 유효한지 항상 확인하는 것이 안전합니다.
            if (IsValid(this))
            {
                // 3초 후에 이 코드가 실행됩니다.
                UE_LOG(LogTemp, Warning, TEXT("This Attack"));
                this->TakeDamage(MockDamageAmount, MockDamageEvent, MockInstigator, MockDamageCauser);
            }
        },
        Delay,
        false // 반복 안 함
    );
    FTimerHandle TestTimerHandl2e;
    GetWorld()->GetTimerManager().SetTimer(
        TestTimerHandl2e,
        [this, MockDamageAmount, MockDamageEvent, MockInstigator, MockDamageCauser]() // 람다 캡처
        {
            // 람다 내부에서는 캡처한 변수가 유효한지 항상 확인하는 것이 안전합니다.
            if (IsValid(this))
            {
                // 3초 후에 이 코드가 실행됩니다.
                UE_LOG(LogTemp, Warning, TEXT("This Attack"));
                this->TakeDamage(MockDamageAmount, MockDamageEvent, MockInstigator, MockDamageCauser);
            }
        },
        6.0f,
        false // 반복 안 함
    );
}

void AEPCombatCharacterBase::InitializeCharacterData()
{
    Super::InitializeCharacterData();

    // 스탯 초기화
    if (StatDataTable && StatComponent) // 캐릭터가 자신의 스탯 데이터 테이블을 가지고 있다고 가정
    {
        // 데이터 테이블에서 Stat 데이터를 찾아옵니다.
        FEPBaseStat* StatData = StatDataTable->FindRow<FEPBaseStat>(StatDataRowName, TEXT(""));
        if (StatData)
        {
            // StatData로 StatComponent 초기화
            StatComponent->Initialize(*StatData);
        }
    }

}

// 피격 몽타주 검색 및 재생
void AEPCombatCharacterBase::HandleHitReaction(EEPHitReactionType HitReactionType)
{
    // 몽타주 검색
    UAnimMontage* ReactionMontage = GetHitReactionMontage(HitReactionType);

    // 몽타주 재생
    if (ReactionMontage)
    {
        PlayAnimMontage(ReactionMontage);
    }
}

// 피격 관련 데이터(FEPDamageInfo) 전달 함수
float AEPCombatCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // FDamageInfo 구조체를 생성하여 모든 데미지 정보 채움
    FEPDamageInfo DamageInfo;
    DamageInfo.BaseDamage = DamageAmount;
    DamageInfo.InstigatorController = EventInstigator;
    DamageInfo.DamageCauser = DamageCauser;
    // DamageEvent로부터 HitLocation, DamageTypeClass 등 추가 정보 추출 가능

    // StatComponent에 모든 정보가 담긴 '사건 파일(DamageInfo)'을 넘겨 처리 위임
    ApplyDamageInfo_Implementation(DamageInfo);

    return DamageInfo.BaseDamage; // 최종적으로 적용된 데미지를 반환 (흡혈에 사용 가능)

}

// 실제 데미지 처리 로직 (계산은 StatComponent에서)
void AEPCombatCharacterBase::ApplyDamageInfo_Implementation(const FEPDamageInfo& DamageInfo)
{
    if (StatComponent && !StatComponent->IsDied())
    {
        StatComponent->ApplyDamage(DamageInfo); 
    }
}

// 죽음 처리 함수
void AEPCombatCharacterBase::HandleDeath_Implementation()
{
    // 데이터 애셋에서 죽음 애니메이션 몽타주를 가져와 재생
    /*if (AnimDataAsset && AnimDataAsset->DeathAnimationMontage)
    {
        PlayAnimMontage(AnimDataAsset->DeathAnimationMontage.LoadSynchronous());
    }*/
    // ...
    if (AnimDataAsset && AnimDataAsset->DeathAnimationMontage.IsValid())
    {
        // 1. 이미 로드되어 있으면 바로 사용
        if (UAnimMontage* Montage = AnimDataAsset->DeathAnimationMontage.Get())
        {
            PlayAnimMontage(Montage);
        }
        else // 2. 로드되어 있지 않다면 (Pending 상태), 비동기 로드를 요청
        {
            // UAssetManager의 싱글턴(전역 인스턴스)를 가져옴
            UAssetManager& AssetManager = UAssetManager::Get();

            // AssetManager를 통해 FStreamableManager에 대한 참조를 얻음
            FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();

            const FSoftObjectPath& AssetPath = AnimDataAsset->DeathAnimationMontage.ToSoftObjectPath();

            // 스트림 관리자에게 "이 애셋 로딩을 시작하고, 로딩이 끝나면 알려줘" 라고 요청
            StreamableManager.RequestAsyncLoad(AssetPath, FStreamableDelegate::CreateUObject(this, &AEPCombatCharacterBase::OnDeathMontageLoaded));
        }
    }

    // 추가적인 죽음 처리 로직 (콜리전 끄기 등)
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// 단순 죽음 애니메이션 실행
void AEPCombatCharacterBase::OnDeathMontageLoaded()
{
    if (UAnimMontage* Montage = AnimDataAsset->DeathAnimationMontage.Get())
    {
        PlayAnimMontage(Montage);
    }
}
// 피격 타입 맞는 몽타주 검색 및 반환 함수
UAnimMontage* AEPCombatCharacterBase::GetHitReactionMontage(EEPHitReactionType HitReactionType)
{
    // 데이터 애셋의 TMap에서 HitReactionType에 맞는 몽타주를 찾아 반환
    if (AnimDataAsset && AnimDataAsset->HitReactionMontages.Contains(HitReactionType))
    {
        return AnimDataAsset->HitReactionMontages[HitReactionType].LoadSynchronous();
    }
    return nullptr;
}

