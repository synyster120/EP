// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EPCombatCharacterBase.h"
#include "Components/EPStatComponent.h"
#include "Components/EPSkillComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/EPCharacterTypes.h"
#include "Core/Helper/EPAsyncLoadHelper.h"
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

    // 캐릭터 데이터를 초기화
    InitializeCharacterData();

    if (StatComponent)
    {
        // 죽음, 피격(애니메이션) 바인딩
        StatComponent->OnDied.AddDynamic(this, &AEPCombatCharacterBase::HandleDeath_Implementation);
        StatComponent->OnHitReact.AddDynamic(this, &AEPCombatCharacterBase::HandleHitReaction);
    }
    
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
    if (!AnimDataAsset || !AnimDataAsset->HitReactionMontages.Contains(HitReactionType))
    {
        return; // 재생할 몽타주가 없으므로 종료
    }

    TSoftObjectPtr<UAnimMontage> MontagePtr = AnimDataAsset->HitReactionMontages[HitReactionType];

    // 몽타주 플레이
    UEPAsyncLoadHelper::RequestAsyncLoad<UAnimMontage>(MontagePtr,
        [this](UAnimMontage* LoadedMontage) // 람다의 파라미터로 로드된 몽타주가 들어옴
        {
            if (LoadedMontage)
            {
                this->PlayAnimMontage(LoadedMontage);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("HitReactionMontages is not set in % s!"), *AnimDataAsset->GetName());
            }
        }
    );
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
    if (AnimDataAsset)
    {
        // "무엇을 로드할지" (DeathAnimationMontage)를 지정
        // "로드가 끝나면 무엇을 할지" (람다 함수)를 직접 전달
        UEPAsyncLoadHelper::RequestAsyncLoad<UAnimMontage>(AnimDataAsset->DeathAnimationMontage,
            [this](UAnimMontage* LoadedMontage) // 람다의 파라미터로 로드된 몽타주가 들어옴
            {
                if (LoadedMontage)
                {
                    PlayAnimMontage(LoadedMontage);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("DeathAnimationMontage is not set in % s!"), *AnimDataAsset->GetName());
                }
            }
        );
    }

    // 추가적인 죽음 처리 로직 (콜리전 끄기 등)
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAnimMontage* AEPCombatCharacterBase::GetHitReactionMontage(EEPHitReactionType HitReactionType)
{
    return nullptr;
}
