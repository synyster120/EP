// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Targeting/EPTargeting_Proximity.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Data/EPSkillTypes.h"
#include "Core/Interfaces/EPCombatQueryInterface.h"


bool UEPTargeting_Proximity::FindTarget(AActor* Caster, const FEPSkillPhaseData& PhaseData, FEPSkillTargetData& OutTargetData)
{
    AActor* OwnerActor = Caster;
    if (!OwnerActor) return false;

    FVector Origin = OwnerActor->GetActorLocation();

    // 인터페이스를 통해 '지정된 방어 지점'이 있는지 확인
    if (Caster->Implements<UEPCombatQueryInterface>())
    {
        IEPCombatQueryInterface* CombatQuery = Cast<IEPCombatQueryInterface>(OwnerActor);
        Origin = CombatQuery->GetMuzzleLocation();
    }

    // 주변 액터 탐색 (Sphere Overlap)
    TArray<AActor*> OverlappedActors;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(OwnerActor); // 나 자신은 제외

    // ObjectTypesToDetect는 에디터에서 설정 (예: Pawn)
    bool bEnemyFound = UKismetSystemLibrary::SphereOverlapActors(
        OwnerActor,
        Origin,
        SearchRadius,
        ObjectTypesToDetect,
        nullptr,
        ActorsToIgnore,
        OverlappedActors
    );

    // ---------------------------------------------------------------------

    // 오버랩 범위 디버그 (빨간색 구체 그리기)
    // PersistSeconds: 2.0f (2초 동안 화면에 남음), Thickness: 2.0f (선 두께)
    //DrawDebugSphere(GetWorld(), Origin, SearchRadius, 12, FColor::Red, false, 2.0f, 0, 2.0f);

    // ---------------------------------------------------------------------

    // 가장 가까운 적 찾기
    AActor* ClosestTarget = nullptr;
    float MinDistanceSq = FLT_MAX;

    if (bEnemyFound)
    {
        for (AActor* Actor : OverlappedActors)
        {
            // 여기서 추가 조건 검사 (예: 살아있는지, 적군인지 등 Interface나 Tag 확인) or teamID 체크하기
            // if (!IsEnemy(Actor)) continue; 

            // 공격 지점이 아닌 "자신"과 가장 가까운 적 저장
            float DistSq = FVector::DistSquared(Origin, Actor->GetActorLocation());
            if (DistSq < MinDistanceSq)
            {
                MinDistanceSq = DistSq;
                ClosestTarget = Actor;
            }
        }
    }


    // ==========================================
    // [분기] 상황에 따라 타겟 타입 변경
    // ==========================================
    if (ClosestTarget)
    {
        // 상황 A: 적을 발견함 -> 유도탄 발사 (Actor 타입)
        OutTargetData.TargetType = EEPTargetType::Actor;
        OutTargetData.TargetActor = ClosestTarget;
        // 적이 있어도 발사 시작 위치나 방향 계산을 위해 Location도 채워주는 게 좋음
        OutTargetData.TargetLocation = ClosestTarget->GetActorLocation();

        UE_LOG(LogTemp, Warning, TEXT("PlayerOffSet %f %f %f"), ClosestTarget->GetActorLocation().X, ClosestTarget->GetActorLocation().Y, ClosestTarget->GetActorLocation().Z);
    }
    else
    {
        // 상황 B: 적이 없음 -> 랜덤 지점 폭격 (Location 타입)
        OutTargetData.TargetType = EEPTargetType::Location;
        OutTargetData.TargetActor = nullptr;

        // 원형 범위 내 랜덤 지점 계산
        // (ReachablePoint를 쓰면 네비게이션 위만 찍고, 그냥 RandomPoint는 공중도 찍힘)
        FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0.0f, SearchRadius);
        RandomOffset.Z = Origin.Z; // 높이는 "공격지점"의 z로 지정
        UE_LOG(LogTemp, Warning, TEXT("OriginOffSet %f %f %f"), Origin.X, Origin.Y, Origin.Z);
        UE_LOG(LogTemp, Warning, TEXT("RandomOffSet %f %f %f"), RandomOffset.X, RandomOffset.Y, RandomOffset.Z);
        OutTargetData.TargetLocation = Origin;// + RandomOffset;
    }

    // 방향(Direction)은 공통적으로 계산
    OutTargetData.TargetDirection = (OutTargetData.TargetLocation - Caster->GetActorLocation()).GetSafeNormal();

    return true; // 무조건 발사는 함 (적에게 쏘든, 허공에 쏘든)
}
