// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EPSkill_Projectile.h"
#include "Skills/EPProjectileBase.h"
#include "Core/Subsystems/EPObjectPoolManager.h"
#include "Data/EPSkillDataAsset.h"
#include "GameFramework/Character.h"
#include "Data/EPObjectPoolTypes.h"
#include "Core/Helper/EPAsyncLoadHelper.h"
#include "Characters/EPCombatCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Gimmick/EPCannon.h"

// 테스트용
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

void UEPSkill_Projectile::Activate(AActor* Caster, const FEPSkillTargetData& NewTargetData, int32 CurrentComboIndex)
{
	Super::Activate(Caster, NewTargetData, CurrentComboIndex);

    if (!Caster || !SkillDataAsset ) return;
    UE_LOG(LogTemp, Warning, TEXT("UEPSkill_Projectile skill is activate - caster : %s"), *Caster->GetName());

    const FEPSkillData& Data = SkillDataAsset->SkillData;
    if (!Data.ComboSequence.IsValidIndex(0)) return;

    // 스킬 데이터에서 현재 콤보 단계에 맞는 데이터 가져옴 (콤보가 아니면 0)
    const FEPSkillPhaseData PhaseData = *GetPhaseData(0);

    // 투사체 클래스 비동기 로드 요청
    UEPAsyncLoadHelper::RequestAsyncLoad<AEPProjectileBase>(PhaseData.ProjectileClass,
        [this, NewTargetData, Caster, PhaseData](TSubclassOf<AEPProjectileBase> LoadedProjectileClass)
        {
            // 투사체 클래스 로드 정상 완료 확인
            if (!LoadedProjectileClass)
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to load SkillClass for %s"), *PhaseData.AnimationTag.ToString());
                return;
            }
            UE_LOG(LogTemp, Warning, TEXT("Success to load SkillClass for %s"), *PhaseData.AnimationTag.ToString());

            FTransform SpawnTransform; // 최종 목적 위치
            FVector LaunchVelocity = FVector::ZeroVector; // 최종 발사 속도를 담을 변수

            const FVector CasterLocation = Caster->GetActorLocation();
            const FRotator CasterRotation = Caster->GetActorRotation();

            // 투사체가 생성될 위치와 방향(Transform)을 계산
            //    (예: 캐릭터의 특정 소켓 위치, 타겟 방향 등)

            // 헬퍼 함수를 호출하여 모든 복잡한 계산을 위임
            if (CalculateLaunchVelocity(Caster, NewTargetData, PhaseData, SpawnTransform, LaunchVelocity))
            {

                // 풀 매니저에게 스폰을 요청
                if (UEPObjectPoolManager* PoolManager = Caster->GetGameInstance()->GetSubsystem<UEPObjectPoolManager>())
                {
                    // TSubclassOf<AEPProjectileBase>를 범용 TSoftClassPtr<AActor>로 변환 후 actor 반환 받음
                    TSoftClassPtr<AActor> ActorClassToSpawn = PhaseData.ProjectileClass;

                    // 스폰 요청 위한 데이터 세팅
                    FEPPoolableObjectInitializer InitializerData;
                    InitializerData.Owner = Caster;
                    InitializerData.Data = this->SkillDataAsset;
                    InitializerData.TargetData = NewTargetData;
                    InitializerData.LaunchVelocity = LaunchVelocity;

                    // 풀 매니저에게 투사체 스폰을 요청
                    //AActor* SpawnedActor = PoolManager2->SpawnObjectFromPool(ActorClassToSpawn, SpawnTransform, InitializerData);

                    //추가된거
                    FTimerHandle SpawnTimerHandle;
                    UEPObjectPoolManager* PoolManager2 = Caster->GetGameInstance()->GetSubsystem<UEPObjectPoolManager>();
                    if (PoolManager)
                    {
                        GetWorld()->GetTimerManager().SetTimer(
                            SpawnTimerHandle,
                            FTimerDelegate::CreateLambda([this, PoolManager2, ActorClassToSpawn, SpawnTransform, InitializerData, PhaseData, Caster]()
                                {
                                    if (!IsValid(this)) return;
                                    if (!IsValid(PoolManager2)) return; // 중요!

                                    FVector ImpactFXVector = Caster->GetActorLocation();
                                    if (AEPCannon* CastingCannon = Cast<AEPCannon>(Caster)) {
                                        UStaticMeshComponent* BodyComp = CastingCannon->GetBodyComponent();
                                        ImpactFXVector = BodyComp->GetSocketLocation(FName("AttackEndSocket"));
                                    }
                                    FRotator FXRotator = FRotator(InitializerData.LaunchVelocity.Rotation().Pitch, InitializerData.LaunchVelocity.Rotation().Yaw, InitializerData.LaunchVelocity.Rotation().Roll);
                                    UE_LOG(LogTemp, Warning, TEXT("HIHI %f %f %f"), FXRotator.Pitch, FXRotator.Yaw, FXRotator.Yaw);
                                    FXRotator = FRotator(-90.f + FXRotator.Pitch, FXRotator.Yaw, 0.f);

                                    UGameplayStatics::PlaySoundAtLocation(GetWorld(), PhaseData.ProjectileInfo.ImpactSound.Get(), ImpactFXVector);
                                    UNiagaraComponent* Comp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PhaseData.ProjectileInfo.ImpactEffect.Get(), ImpactFXVector, FXRotator);
                                    if (Comp)
                                    {
                                        Comp->SetWorldRotation(FXRotator);
                                    }
                                    AActor* SpawnedActor = PoolManager2->SpawnObjectFromPool(ActorClassToSpawn, SpawnTransform, InitializerData);
                                }),
                            1.0f,
                            false
                        );
                    }

                    //AEPProjectileBase* Projectile = Cast<AEPProjectileBase>(SpawnedActor);

                    //if (Projectile && Projectile->GetProjectileMovementComponent())
                    //{
                    //    // 최종적으로 계산된 속도를 투사체에 적용
                    //    Projectile->GetProjectileMovementComponent()->Velocity = LaunchVelocity;
                    //}
                }
            }


            // -------------- 투사체 요청 --------------
                // 풀 매니저를 가져옴
            /*if (UEPObjectPoolManager* PoolManager = Caster->GetGameInstance()->GetSubsystem<UEPObjectPoolManager>())
            {
                // TSubclassOf<AEPProjectileBase>를 범용 TSoftClassPtr<AActor>로 변환 후 actor 반환 받음
                TSoftClassPtr<AActor> ActorClassToSpawn = PhaseData.ProjectileClass;
                //AActor* SpawnedActor = PoolManager->GetObjectFromPool(ActorClassToSpawn);

                // 스폰 요청 위한 데이터 세팅
                FEPPoolableObjectInitializer InitializerData;
                InitializerData.Owner = Caster;
                InitializerData.Data = SkillDataAsset;
                InitializerData.TargetData = NewTargetData;

                // 풀 매니저에게 투사체 스폰을 요청
                PoolManager->SpawnObjectFromPool(ActorClassToSpawn, SpawnTransform, InitializerData);
            }*/

            // -------------- 애니메이션 재생 요청 --------------
            AEPCombatCharacterBase* Character = Cast<AEPCombatCharacterBase>(Caster);
            if (!Character) return;

            Character->PlayAnimationByTag(PhaseData.AnimationTag);
            UE_LOG(LogTemp, Warning, TEXT("Skill_projectile --> playing animation : %s"), *PhaseData.AnimationTag.ToString());

        });

}

// target data 처리
bool UEPSkill_Projectile::CalculateLaunchVelocity(AActor* Caster, const FEPSkillTargetData& TargetData, const FEPSkillPhaseData& PhaseData, FTransform& OutSpawnTransform, FVector& OutLaunchVelocity) const
{
    if (!Caster) return false;

    // 발사 시작 위치 (Muzzle) 결정
    // 인터페이스를 통해 총구/손 위치를 가져오고, 없으면 몸통 앞 50cm
    FVector StartLocation = Caster->GetActorLocation();
    /*if (Caster->Implements<UEPCombatQueryInterface>())
    {
        StartLocation = IEPCombatQueryInterface::Execute_GetMuzzleLocation(Caster);
    }
    else*/
    {
        StartLocation += Caster->GetActorForwardVector() * 50.0f;
    }

    // 데이터 테이블에서 설정한 투사체 속력
    const float LaunchSpeed = PhaseData.ProjectileInfo.InitialSpeed;
    bool bCalculationSuccess = false; // 계산 성공 여부

    // -------------- 생성위치/발사체속도 계산 --------------
    switch (TargetData.TargetType)
    {
    case EEPTargetType::Direction:
    {
        // [방향 발사] 타겟 데이터의 방향 or 캐스터 정면
        FVector FireDir = TargetData.TargetDirection.IsZero() ? Caster->GetActorForwardVector() : TargetData.TargetDirection;

        // 방향 * 속력 = 속도 벡터
        OutLaunchVelocity = FireDir * LaunchSpeed;
        OutSpawnTransform = FTransform(FireDir.Rotation(), StartLocation);
        bCalculationSuccess = true;
        break;
    }
    case EEPTargetType::Actor:
    case EEPTargetType::Location:
    {
        // [지점 발사] Actor와 Location 모두 '목표 지점'이 존재함
        FVector EndLocation;
        if (TargetData.TargetType == EEPTargetType::Actor && TargetData.TargetActor)
        {
            EndLocation = TargetData.TargetActor->GetActorLocation();
        }
        else
        {
            EndLocation = TargetData.TargetLocation;
        }
        //UE_LOG(LogTemp, Warning, TEXT("StartZ=%.1f EndZ(Origin)=%.1f"), StartLocation.Z, EndLocation.Z);

        // 만약 대포면 방향 먼저 알려주기
        if (AEPCannon* CastingCannon = Cast<AEPCannon>(Caster)) {
            UStaticMeshComponent* BodyComp = CastingCannon->GetBodyComponent();
            FVector SocketStart = BodyComp->GetSocketLocation(FName("AttackStartSocket"));
            FVector SocketEnd = BodyComp->GetSocketLocation(FName("AttackEndSocket"));

            FRotator LookAtRotHor = UKismetMathLibrary::FindLookAtRotation(SocketStart, EndLocation);

            UGameplayStatics::SuggestProjectileVelocity(
                this,
                OutLaunchVelocity,
                SocketStart,
                EndLocation,
                LaunchSpeed,
                false, 0.0f, 0.0f,
                ESuggestProjVelocityTraceOption::DoNotTrace
            );
            CastingCannon->AimTarget(OutLaunchVelocity.Rotation().Yaw, OutLaunchVelocity.Rotation().Pitch);
            FVector Direction = OutLaunchVelocity.GetSafeNormal();
            FVector NewStart = SocketStart + Direction * 114.f;
            StartLocation = NewStart;
        }

        // 곡사포(Arc) 궤적 계산 시도
        // 물리 엔진이 목표 지점에 도달하기 위한 속도 벡터(OutLaunchVelocity)를 계산해줌
        bCalculationSuccess = UGameplayStatics::SuggestProjectileVelocity(
            this,
            OutLaunchVelocity,
            StartLocation,
            EndLocation,
            LaunchSpeed,
            false, 0.0f, 0.0f,
            ESuggestProjVelocityTraceOption::DoNotTrace
        );

        // 곡사 계산 실패 시 (사거리 부족, 각도 안 나옴 등) -> 직사(Linear)로 전환
        if (!bCalculationSuccess)
        {
            // 목표 지점을 향한 직선 방향 벡터 추출 (SafeNormal로 안전하게)
            FVector DirectDir = (EndLocation - StartLocation).GetSafeNormal();
            OutLaunchVelocity = DirectDir * LaunchSpeed;
            bCalculationSuccess = true; // 직사로라도 쏘니까 성공 처리
        }

        // 발사체가 날아가는 방향을 바라보게 회전 설정
        OutSpawnTransform = FTransform(OutLaunchVelocity.Rotation(), StartLocation);
        break;
    }
    default:
        // 예외 상황: 그냥 정면 발사
        OutLaunchVelocity = Caster->GetActorForwardVector() * LaunchSpeed;
        OutSpawnTransform = FTransform(OutLaunchVelocity.Rotation(), StartLocation);
        bCalculationSuccess = true;
        break;
    }

    return bCalculationSuccess;
}
