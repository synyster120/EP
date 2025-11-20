#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "EPNotifyState_PlayerDissolve.generated.h"

class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;

UCLASS()
class STARBOARDMAP_API UEPNotifyState_PlayerDissolve : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    // -1 → 1 이런 식으로 시작/끝 값
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolve")
    float StartValue = -1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolve")
    float EndValue = 1.0f;

    // 어떤 MPC를 쓸지 (에디터에서 MPC_Player_Dissolve 지정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolve")
    TObjectPtr<UMaterialParameterCollection> DissolveCollection;

    // MPC 안의 파라미터 이름 (보통 "Dissolve")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolve")
    FName ParamName = TEXT("Dissolve");

private:
    float AccumTime = 0.0f;
    float Duration = 0.0f;

    // 런타임에서 쓰는 MPC 인스턴스
    UMaterialParameterCollectionInstance* CollectionInstance = nullptr;

public:
    virtual void NotifyBegin(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float TotalDuration,
        const FAnimNotifyEventReference& EventReference
    ) override;

    virtual void NotifyTick(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float FrameDeltaTime,
        const FAnimNotifyEventReference& EventReference
    ) override;

    virtual void NotifyEnd(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference
    ) override;
};
