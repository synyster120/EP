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
#include "Sound/SoundBase.h" // sound
#include "Core/Helper/EPAsyncLoadHelper.h" // helper

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
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEPProjectileBase::OnProjectileOverlap);
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
    if (!Initializer.Data || !ProjectileMovementComponent) return;

    // 계산된 '완성된 속도 벡터' 적용
    ProjectileMovementComponent->Velocity = Initializer.LaunchVelocity;

    // 데이터 테이블 정보 적용 (속력, 중력 등)
    if (Initializer.Data->Implements<UEPProjectileDataProvider>()) // 발사체가 있는 스킬단계인지 확인
    {
        IEPProjectileDataProvider::Execute_GetPhaseDataFromAsset(Initializer.Data, 0, this->PhaseData);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ERROR :: AEPProjectileBase --> poolable initialize --> get phase data load fail"));
    }
    const FEPProjectileData& Info = this->PhaseData.ProjectileInfo; // 가져온 데이터 저장

    // 적용
    ProjectileMovementComponent->InitialSpeed = Info.InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = Info.MaxSpeed;
    ProjectileMovementComponent->ProjectileGravityScale = Info.GravityScale;
    ProjectileMovementComponent->bShouldBounce = Info.bCanBounce;

    // 유도탄(Homing) 과 일반탄(Ballistic) 분기 설정
    const bool bIsHoming = (Initializer.TargetData.TargetType == EEPTargetType::Actor);
    ProjectileMovementComponent->bIsHomingProjectile = bIsHoming; // 유도 설정

    if (bIsHoming) // [유도탄 설정]
    {
        
        // 타겟 액터가 존재할 때만 유도 컴포넌트 설정
        /*if (Initializer.TargetData.TargetActor)
        {
            ProjectileMovementComponent->HomingTargetComponent = Initializer.TargetData.TargetActor->GetRootComponent();
        }

        // 유도 성능 설정
        ProjectileMovementComponent->HomingAccelerationMagnitude = Info.HomingMagnitude;*/
    }
    else // [일반탄 설정]
    {
        // 중력을 꺼주기(직사일 경우)
        if (Initializer.TargetData.TargetType == EEPTargetType::Direction) ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    }

    // 투사체의 주인을 설정
    SetOwner(Initializer.Owner);
    bIsValid = true; // 초기화 완료 저장
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

    CollisionSphere->SetCollisionProfileName(TEXT("OverlapAll"));
    SetActorEnableCollision(true);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

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
    //CollisionSphere->OnComponentBeginOverlap.Clear();
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


    //UE_LOG(LogTemp, Warning, TEXT("[ %s ] ok OnOverlap --> takedamage pless"), *SweepResult.GetActor()->GetName());

    // OnExpire가 호출되기 전에 타이머를 명시적으로 취소
    GetWorld()->GetTimerManager().ClearTimer(LifespanTimer);

    // 충돌 관련 로직 (데미지 전달, 이펙트 재생 등)
    // PhaseData를 참조하여 데미지와 충돌 이펙트를 가져와 사용합니다.
    if (bIsValid && OtherActor != GetOwner())
    {
        /* 주석 원래 코드
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
        }*/

        //캐논폭발
        SetActorHiddenInGame(true);
        CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        const FVector ExplosionLocation = this->GetActorLocation();
#if !(UE_BUILD_SHIPPING)
        //DrawDebugSphere(this->GetWorld(), ExplosionLocation, 200.f, 24, FColor::Red, false, 2.f, 0, 2.f);
#endif
        FGenericTeamId CasterTeamId = FGenericTeamId::NoTeam; // 기본값 = '팀 없음'
        if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(GetOwner()))
        {
            CasterTeamId = TeamAgent->GetGenericTeamId();
        }

        TArray<AActor*> OverlappedActors;
        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
        UKismetSystemLibrary::SphereOverlapActors(
            this->GetWorld(),
            ExplosionLocation,
            200.f,
            ObjectTypes,
            AActor::StaticClass(),
            TArray<AActor*>(),
            OverlappedActors
        );

        if (CasterTeamId != FGenericTeamId::NoTeam) // 시전자가 팀이 있을 경우에만 검사
        {
            for (AActor* OverlappedActor : OverlappedActors)
            {
                IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(OverlappedActor);
                // 대상이 같은 팀이라면, 무시 목록에 추가합니다.
                if (TargetTeamAgent && TargetTeamAgent->GetGenericTeamId() != CasterTeamId)
                {
                    UGameplayStatics::ApplyDamage(
                        OverlappedActor,
                        20.f,
                        nullptr,
                        GetOwner(),
                        UDamageType::StaticClass()
                    );
                }
            }
        }

        //USoundBase* SFXAsset = PhaseData.SFX.LoadSynchronous(); // 강제 동기 로드
        //if (SFXAsset) {
        //    UGameplayStatics::PlaySoundAtLocation(GetWorld(), SFXAsset, GetActorLocation());
        //}
        
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), PhaseData.SFX.Get(), GetActorLocation());

        //UNiagaraSystem* Asset = PhaseData.VFX.LoadSynchronous(); // 강제 동기 로드
        //if (Asset) {
        //    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Asset, GetActorLocation());
        //}
        
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PhaseData.VFX.Get(), GetActorLocation());


        // 비동기 로드 로드 (헬퍼 사용)
        //UEPAsyncLoadHelper::RequestAsyncLoad<USoundBase>(PhaseData.SFX,
        //    [this](USoundBase* LoadedSoundBase) // 람다의 파라미터로 로드된 몽타주가 들어옴
        //    {
        //        if (!LoadedSoundBase) return;

        //        UGameplayStatics::PlaySoundAtLocation(GetWorld(), PhaseData.SFX.Get(), GetActorLocation());
        //    }
        //);

        //// 비동기 로드 로드 (헬퍼 사용)
        //UEPAsyncLoadHelper::RequestAsyncLoad<UNiagaraSystem>(PhaseData.VFX,
        //    [this](UNiagaraSystem* LoadedNiagaraSystem) // 람다의 파라미터로 로드된 몽타주가 들어옴
        //    {
        //        if (!LoadedNiagaraSystem) return;

        //        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PhaseData.VFX.Get(), GetActorLocation());
        //    }
        //);
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

