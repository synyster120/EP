#include "EPNotifyState_PlayerDissolve.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

void UEPNotifyState_PlayerDissolve::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    AccumTime = 0.0f;
    Duration = TotalDuration;
    CollectionInstance = nullptr;

    if (!MeshComp || !DissolveCollection)
    {
        return;
    }

    UWorld* World = MeshComp->GetWorld();
    if (!World)
    {
        return;
    }

    // 현재 월드에서 MPC 인스턴스 가져오기
    CollectionInstance = World->GetParameterCollectionInstance(DissolveCollection);
    if (CollectionInstance)
    {
        // 시작 값으로 세팅
        CollectionInstance->SetScalarParameterValue(ParamName, StartValue);
    }
}

void UEPNotifyState_PlayerDissolve::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

    if (!CollectionInstance)
    {
        return;
    }

    AccumTime += FrameDeltaTime;

    float Alpha = 1.0f;
    if (Duration > KINDA_SMALL_NUMBER)
    {
        Alpha = FMath::Clamp(AccumTime / Duration, 0.0f, 1.0f);
    }

    const float Current = FMath::Lerp(StartValue, EndValue, Alpha);

    // MPC의 Dissolve 값 갱신
    CollectionInstance->SetScalarParameterValue(ParamName, Current);
}

void UEPNotifyState_PlayerDissolve::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (CollectionInstance)
    {
        // 끝 값으로 한 번 더 세팅 (취향에 따라 0 으로 리셋해도 됨)
        CollectionInstance->SetScalarParameterValue(ParamName, EndValue);
        CollectionInstance = nullptr;
    }

    AccumTime = 0.0f;
    Duration = 0.0f;
}
