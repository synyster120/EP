// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Data/SkillTypes.h" 
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"

AProjectileBase::AProjectileBase()
{
 	PrimaryActorTick.bCanEverTick = true;

	// 처음 스폰될 때는 기본적으로 Tick이 '꺼진 상태'로 시작
	//PrimaryActorTick.bStartWithTickEnabled = false;

	bIsActive = false;
    bIsValid = false;

	// ... 컴포넌트 생성 로직

    // 콜리전 스피어 생성
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;
    CollisionSphere->SetCollisionProfileName(TEXT("Projectile"));

}

// 스킬 단계 데이터로 초기화 함수
void AProjectileBase::Initialize(const FSkillPhaseData* InPhaseData, AActor* InOwner)
{
    if (!InPhaseData) return;

    // 전달받은 데이터 전체를 멤버 변수에 저장
    PhaseData = *InPhaseData;

    // 투사체의 주인을 설정
    SetOwner(InOwner);

    // MovementComponent의 '설정값'들을 미리 세팅
    if (MovementComponent)
    {
        const FProjectileData& ProjectileInfo = PhaseData.ProjectileInfo;
        MovementComponent->InitialSpeed = ProjectileInfo.InitialSpeed;
        MovementComponent->MaxSpeed = ProjectileInfo.MaxSpeed;
        MovementComponent->ProjectileGravityScale = ProjectileInfo.GravityScale;
    }

    bIsValid = true;
}

// Interface - Pool에 요청
void AProjectileBase::Activate()
{
    if (bIsActive) return;
    bIsActive = true;

    // 액터를 보이게 하고, 충돌 및 Tick을 활성화
    SetActorHiddenInGame(false);
    SetActorTickEnabled(true);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // ProjectileMovementComponent를 활성화하고, 저장된 속성으로 '움직임'을 시작
    if (MovementComponent && bIsValid)
    {
        MovementComponent->Activate();

        // 바로 이 Activate 함수에서 Velocity를 설정하여 움직임을 시작시킵니다.
        MovementComponent->Velocity = GetActorForwardVector() * PhaseData.ProjectileInfo.InitialSpeed;
    }

    // 수명(LifeSpan)이 설정되어 있다면, '활성화'된 이 시점부터 소멸 타이머를 예약
    if (bIsValid && PhaseData.ProjectileInfo.LifeSpan > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(LifespanTimer, this, &AProjectileBase::OnExpire, PhaseData.ProjectileInfo.LifeSpan, false);
    }
}

float AProjectileBase::BeginDeactivate()
{
    // 더 이상 움직이거나 부딪히지 않도록 즉시 비활성화 (Mesh 숨기기 필요)
    SetActorEnableCollision(ECollisionEnabled::NoCollision);
    if (auto* MovementComp = FindComponentByClass<UProjectileMovementComponent>())
    {
        MovementComp->StopMovementImmediately();
    }

    // 소멸 이펙트를 재생
    //PlayFizzleEffect();

    // Initialize 때 받아두었던 스킬 데이터에서 지연 시간 값을 읽어와 반환
    if (bIsValid) // Initialize 때 저장해 둔 FSkillPhaseData 포인터
    {
        return PhaseData.DeactivationDelay;
    }

    return 0.0f; // 데이터가 없으면 즉시 소멸
}

// Interface - Pool이 호출하는 Actor의 비활성화만 처리하는 함수
void AProjectileBase::Deactivate()
{
    if (!bIsActive) return;
    bIsActive = false;

    // 비활성화에 필요한 모든 '구현'
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (MovementComponent)
    {
        MovementComponent->StopMovementImmediately();
        MovementComponent->Velocity = FVector::ZeroVector;
    }
    // 진행 중인 모든 타이머를 확실히 정리
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // OnExpire가 호출되기 전에 타이머를 명시적으로 취소
    GetWorld()->GetTimerManager().ClearTimer(LifespanTimer);

    // 충돌 관련 로직 (데미지 전달, 이펙트 재생 등)
    // PhaseData를 참조하여 데미지와 충돌 이펙트를 가져와 사용합니다.
    if (bIsValid && OtherActor != GetOwner())
    {
        // 데미지 전달 로직
        UGameplayStatics::ApplyDamage(OtherActor, PhaseData.Damage, GetOwner()->GetInstigatorController(), this, nullptr);
        // 충돌 이펙트 재생 로직
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PhaseData.VFX.Get(), GetActorLocation());
    }

    // 모든 처리가 끝나면 풀에 반납 요청
    OnReturnToPool();
}

// 발사체의 생명주기 종료 시 호출 함수 (발사 후 일정시간 지나면 사라짐)
void AProjectileBase::OnExpire()
{
    // ... 소멸(Fizzle) 이펙트 재생 로직

    OnReturnToPool(); // Mananger에 반납 요청 호출(OnHit와 동일)
}

// Pool에 반납 요청 함수
void AProjectileBase::OnReturnToPool()
{
    if (OwnerPool)
    {
        // ReturnObjectToPool() : 투사체에게 소멸 준비를 시작하라고 통지 후 대기시간 동안 대기
        //OwnerPool->ReturnObjectToPool(this);
    }
}

void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

