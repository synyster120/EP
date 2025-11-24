// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Data/EPSkillTypes.h" 
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Data/EPObjectPoolTypes.h"
#include "GenericTeamAgentInterface.h" 

AEPProjectileBase::AEPProjectileBase()
{
 	PrimaryActorTick.bCanEverTick = true;
    
	// 처음 스폰될 때는 기본적으로 Tick이 '꺼진 상태'로 시작 but 기본은 켜두는 게 좋음
	//PrimaryActorTick.bStartWithTickEnabled = false;

	bIsActive = true;
    bIsValid = false;

    // 콜리전 스피어 생성
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;
    // 메시 컴포넌트 생성
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);

    // 투사체 Movement 컴포넌트 생성
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->SetUpdatedComponent(CollisionSphere);

    // 콜리전 프로파일 설정
    CollisionSphere->SetCollisionProfileName(TEXT("Projectile"));
    MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
    // Overlap 이벤트 활성화
    CollisionSphere->SetGenerateOverlapEvents(true);
}

// 스킬 단계 데이터로 초기화 함수 ================================= 사용 안함 =================================
void AEPProjectileBase::Initialize(const FEPSkillPhaseData* InPhaseData, AActor* InOwner)
{
    //if (!InPhaseData) return;

    //// 전달받은 데이터 전체를 멤버 변수에 저장
    //PhaseData = *InPhaseData;

    //// 투사체의 주인을 설정
    //SetOwner(InOwner);

    //// ProjectileMovementComponent의 '설정값'들을 미리 세팅
    //if (ProjectileMovementComponent)
    //{
    //    const FEPProjectileData& ProjectileInfo = PhaseData.ProjectileInfo;
    //    ProjectileMovementComponent->InitialSpeed = ProjectileInfo.InitialSpeed;
    //    ProjectileMovementComponent->MaxSpeed = ProjectileInfo.MaxSpeed;
    //    ProjectileMovementComponent->ProjectileGravityScale = ProjectileInfo.GravityScale; 
    //    ProjectileMovementComponent->bShouldBounce = ProjectileInfo.bCanBounce;

    //    //ProjectileMovementComponent->bIsHomingProjectile = ProjectileInfo.bIsHoming; // 유도 여부
    //}

    //bIsValid = true;
}

// 데이터 초기화 함수
void AEPProjectileBase::PoolableInitialize_Implementation(const FEPPoolableObjectInitializer& Initializer)
{
    if (!&Initializer) return;

    // 전달받은 데이터 애셋이 '투사체 데이터 제공자' 인터페이스를 가지고 있는지 확인
    if (Initializer.Data && Initializer.Data->Implements<UEPProjectileDataProvider>())
    {
        // 인터페이스를 통해 안전하게 데이터 '요청'
        if (IEPProjectileDataProvider::Execute_GetPhaseDataFromAsset(Initializer.Data, 0, this->PhaseData)) // 전달받은 데이터를 멤버 변수 PhaseData에 저장
        {
            if (ProjectileMovementComponent)
            {
                const FEPProjectileData& ProjectileInfo = this->PhaseData.ProjectileInfo;

                // TargetType을 보고 유도 여부를 '결정'
                const bool bShouldHoming = (Initializer.TargetData.TargetType == EEPTargetType::Actor);
                ProjectileMovementComponent->bIsHomingProjectile = bShouldHoming;

                // === 받아온 데이터로 자신 초기화 ===

                // 만약 유도탄이라면, 목표물과 성능을 '설정'
                if (bShouldHoming)
                {
                    if (Initializer.TargetData.TargetActor)
                    {
                        ProjectileMovementComponent->HomingTargetComponent = Initializer.TargetData.TargetActor->GetRootComponent();
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("TargetActor is null "));
                    }
                    ProjectileMovementComponent->HomingAccelerationMagnitude = ProjectileInfo.HomingMagnitude;
                    //ProjectileMovementComponent->Velocity = GetActorForwardVector() * this->PhaseData.ProjectileInfo.InitialSpeed;
                }

                // 속도 지정
                InitializeLaunchVelocity = Initializer.LaunchVelocity;
                ProjectileMovementComponent->Velocity = InitializeLaunchVelocity;

                // 그 외 속도, 중력 등 나머지 모든 값을 설정
                ProjectileMovementComponent->InitialSpeed = ProjectileInfo.InitialSpeed;
                ProjectileMovementComponent->MaxSpeed = ProjectileInfo.MaxSpeed;
                ProjectileMovementComponent->ProjectileGravityScale = ProjectileInfo.GravityScale;
                ProjectileMovementComponent->bShouldBounce = ProjectileInfo.bCanBounce;
            }

            // ...
            //Damage = ImpactInfo.Damage; // 데미지 등 충돌 정보도 저장
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ERROR :: AEPProjectileBase --> poolable initialize --> get phase data load fail"));
        }

    }

    // 투사체의 주인을 설정
    SetOwner(Initializer.Owner);
    bIsValid = true;
}

// Interface - Pool에 요청
void AEPProjectileBase::Activate()
{
    if (bIsActive) return;
    bIsActive = true;

    UE_LOG(LogTemp, Warning, TEXT("EP_Error:: AEPProjectileBase -- Activate"));

    // 액터를 보이게 하고, 충돌 및 Tick을 활성화
    SetActorHiddenInGame(false);
    SetActorTickEnabled(true);

    //ollisionSphere->SetCollisionProfileName(TEXT("Projectile"));
    SetActorEnableCollision(true);
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEPProjectileBase::OnProjectileOverlap);

    // ProjectileMovementComponent를 활성화하고, 저장된 속성으로 '움직임'을 시작
    if (ProjectileMovementComponent && bIsValid)
    {
        //  움직일 대상 재지정(이유 : Block충돌 시 내부적 중단->대상 참조도 함께 비활성화(초기화))
        ProjectileMovementComponent->SetUpdatedComponent(RootComponent);

        ProjectileMovementComponent->Activate();

        //ProjectileMovementComponent->Velocity = InitializeLaunchVelocity;
    }

    // 수명(LifeSpan)이 설정되어 있다면, '활성화'된 이 시점부터 소멸 타이머를 예약
    if (bIsValid && PhaseData.ProjectileInfo.LifeSpan > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(LifespanTimer, this, &AEPProjectileBase::OnExpire, PhaseData.ProjectileInfo.LifeSpan, false);
    }
}

float AEPProjectileBase::BeginDeactivate()
{
    UE_LOG(LogTemp, Error, TEXT("BeginDeactivate!"));
    // 더 이상 움직이거나 부딪히지 않도록 비활성화
    SetActorEnableCollision(ECollisionEnabled::NoCollision);

    // 소멸 이펙트를 재생
    //PlayFizzleEffect();

    // Initialize 때 받아두었던 스킬 데이터에서 지연 시간 값을 읽어와 반환
    if (bIsValid) // Initialize 때 저장해 둔 FEPSkillPhaseData 포인터
    {
        return PhaseData.DeactivationDelay;
    }

    return 0.0f; // 데이터가 없으면 즉시 소멸
}

// Interface - Pool이 호출하는 Actor의 비활성화만 처리하는 함수
void AEPProjectileBase::Deactivate()
{
    UE_LOG(LogTemp, Error, TEXT("Deactivate!"));
    if (!bIsActive) return;
    bIsActive = false;

    // 비활성화에 필요한 모든 '구현'
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // 바인딩 제거
    CollisionSphere->OnComponentBeginOverlap.Clear();
    //CollisionSphere->OnComponentBeginOverlap.RemoveDynamic(this, &AEPProjectileBase::OnProjectileOverlap);
    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->StopMovementImmediately();
        //ProjectileMovementComponent->Velocity = FVector::ZeroVector;
    }
    // 진행 중인 모든 타이머를 확실히 정리
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

UProjectileMovementComponent* AEPProjectileBase::GetProjectileMovementComponent()
{ 
    return ProjectileMovementComponent; 
}

void AEPProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}
//
//void AEPProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//    UE_LOG(LogTemp, Warning, TEXT("ok OnHit --> takedamage pless"));
//
//    // OnExpire가 호출되기 전에 타이머를 명시적으로 취소
//    GetWorld()->GetTimerManager().ClearTimer(LifespanTimer);
//
//    // 충돌 관련 로직 (데미지 전달, 이펙트 재생 등)
//    // PhaseData를 참조하여 데미지와 충돌 이펙트를 가져와 사용합니다.
//    if (bIsValid && OtherActor != GetOwner())
//    {
//        // 데미지 전달 로직
//        UGameplayStatics::ApplyDamage(OtherActor, PhaseData.Damage, GetOwner()->GetInstigatorController(), this, nullptr);
//        // 충돌 이펙트 재생 로직
//        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PhaseData.VFX.Get(), GetActorLocation());
//    }
//
//    // 모든 처리가 끝나면 풀에 반납 요청
//    OnReturnToPool();
//}

// 발사체의 생명주기 종료 시 호출 함수 (발사 후 일정시간 지나면 사라짐)
void AEPProjectileBase::OnExpire()
{
    // ... 소멸(Fizzle) 이펙트 재생 로직

    OnReturnToPool(); // Mananger에 반납 요청 호출(OnHit와 동일)
}

void AEPProjectileBase::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }


    UE_LOG(LogTemp, Warning, TEXT("[ %s ] ok OnOverlap --> takedamage pless"), *SweepResult.GetActor()->GetName());

    // OnExpire가 호출되기 전에 타이머를 명시적으로 취소
    GetWorld()->GetTimerManager().ClearTimer(LifespanTimer);

    // 충돌 관련 로직 (데미지 전달, 이펙트 재생 등)
    // PhaseData를 참조하여 데미지와 충돌 이펙트를 가져와 사용합니다.
    if (bIsValid && OtherActor != GetOwner())
    {
        // 팀 구별
        FGenericTeamId CasterTeamId = FGenericTeamId::NoTeam; // 기본값 = '팀 없음'
        if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(GetOwner()))
        {
            CasterTeamId = TeamAgent->GetGenericTeamId();
        }

        // perception actor 가 같은 팀인지 확인
        if (CasterTeamId != FGenericTeamId::NoTeam) // 시전자가 팀이 있을 경우에만 검사
        {
            IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(OtherActor);
            // 대상이 다른 팀이라면
            if (TargetTeamAgent && TargetTeamAgent->GetGenericTeamId() != CasterTeamId)
            {
                // 데미지 전달 로직
                UGameplayStatics::ApplyDamage(OtherActor, PhaseData.Damage, GetOwner()->GetInstigatorController(), this, nullptr);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[ %s ] ok OnOverlap --> team is true, applydamage fail "), *SweepResult.GetActor()->GetName());
            }
        }
        // 충돌 이펙트 재생 로직
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PhaseData.VFX.Get(), GetActorLocation());
    }

    // 모든 처리가 끝나면 풀에 반납 요청
    OnReturnToPool();
}

// Pool에 반납 요청 함수
void AEPProjectileBase::OnReturnToPool()
{
    if (OwnerPool)
    {
        // ReturnObjectToPool() : 투사체에게 소멸 준비를 시작하라고 통지 후 대기시간 동안 대기
        OwnerPool->ReturnObjectToPool(this);
    }
}

void AEPProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

