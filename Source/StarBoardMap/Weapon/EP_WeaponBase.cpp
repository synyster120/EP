// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/EP_WeaponBase.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AEP_WeaponBase::AEP_WeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AEP_WeaponBase::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle AttachDelayHandle;
    GetWorld()->GetTimerManager().SetTimer(
        AttachDelayHandle,
        this,
        &AEP_WeaponBase::AttachToCharacter,
        0.1f,
        false
    );
}

void AEP_WeaponBase::AttachToCharacter()
{
    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    USkeletalMeshComponent* MeshComp = MyCharacter->GetMesh();
    UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
    if (!AnimInst) return;

    //FName VarName("IsGotHammer");
    //FProperty* Property = AnimInst->GetClass()->FindPropertyByName(VarName);
    //if (Property)
    //{
    //    FBoolProperty* FloatProp = CastField<FBoolProperty>(Property);
    //    if (FloatProp)
    //    {
    //        FloatProp->SetPropertyValue_InContainer(AnimInst, true);
    //    }
    //}

    if (UStaticMeshComponent* ActorMesh = this->FindComponentByClass<UStaticMeshComponent>())
    {
        ActorMesh->SetSimulatePhysics(false);
        ActorMesh->SetEnableGravity(false);
        ActorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        ActorMesh->SetRelativeLocation(FVector::ZeroVector);
        ActorMesh->SetRelativeRotation(FRotator::ZeroRotator);
        this->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HammerSocket"));


        this->SetActorRelativeRotation(FRotator::MakeFromEuler(FVector(-90.f, -20.f, 0.f)));
    }
}

void AEP_WeaponBase::DetachFromCharacter()
{
    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    USkeletalMeshComponent* MeshComp = MyCharacter->GetMesh();
    UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
    if (!AnimInst) return;

    // Player 의 ABP 변수 변경
    FName VarName("IsGotHammer");
    FProperty* Property = AnimInst->GetClass()->FindPropertyByName(VarName);
    if (Property)
    {
        FBoolProperty* FloatProp = CastField<FBoolProperty>(Property);
        if (FloatProp)
        {
            FloatProp->SetPropertyValue_InContainer(AnimInst, false);
        }
    }

    this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    // 자신의 StaticMesh에 물리 적용
    if (UStaticMeshComponent* ActorMesh = this->FindComponentByClass<UStaticMeshComponent>())
    {
        ActorMesh->SetSimulatePhysics(true);
        ActorMesh->SetEnableGravity(true);
        ActorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        FVector Forward = MyCharacter->GetActorForwardVector();
        FVector Impulse = Forward * 100.f + FVector(0, 0, 200.f);
        ActorMesh->AddImpulse(Impulse, NAME_None, true);
    }

}