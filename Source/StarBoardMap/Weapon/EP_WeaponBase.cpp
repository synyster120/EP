// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/EP_WeaponBase.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UEP_WeaponBase::UEP_WeaponBase()
{
	
}


// Called when the game starts
void UEP_WeaponBase::BeginPlay()
{
	Super::BeginPlay();

    SpawnWeapon(FName("Hammer"));
}

void UEP_WeaponBase::SpawnWeapon(FName SpawnWeaponName)
{
    WeaponName = SpawnWeaponName;

    FTimerHandle AttachDelayHandle;
    GetWorld()->GetTimerManager().SetTimer(
        AttachDelayHandle,
        this,
        &UEP_WeaponBase::StartWithCharacter,
        0.1f,
        false
    );
}

void UEP_WeaponBase::StartWithCharacter()
{
    AActor* Owner = GetOwner();
    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    USkeletalMeshComponent* MeshComp = MyCharacter->GetMesh();
    UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
    if (!AnimInst) return;

    // "Speed"는 블루프린트 변수 이름과 정확히 일치해야 함
    FName VarName("IsGotHammer");

    // 변수 찾기
    FProperty* Property = AnimInst->GetClass()->FindPropertyByName(VarName);
    if (Property)
    {
        // float 타입 변수에 접근하는 예시
        FBoolProperty* FloatProp = CastField<FBoolProperty>(Property);
        if (FloatProp)
        {
            FloatProp->SetPropertyValue_InContainer(AnimInst, true);
        }
    }


    Owner->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HammerSocket"));
   
    FRotator DesiredWorldRot = FRotator(-90.f, -20.f, 0);

    
    Owner->SetActorRelativeRotation(FRotator::MakeFromEuler(FVector(-90.f, -20.f, 0.f)));
}
