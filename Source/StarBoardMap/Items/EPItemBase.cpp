
#include "Items/EPItemBase.h"

AEPItemBase::AEPItemBase()
{
	PrimaryActorTick.bCanEverTick = true;

#if WITH_EDITORONLY_DATA 
    PreviewMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMeshComp"));
    RootComponent = PreviewMeshComp;

    // 껍데기 만들기
    PreviewMeshComp->SetCollisionProfileName(TEXT("NoCollision")); // 충돌 끄기
    PreviewMeshComp->SetGenerateOverlapEvents(false); // 오버랩 끄기
    PreviewMeshComp->SetSimulatePhysics(false); // 물리 끄기
#endif
}

void AEPItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// 에디터내에서 액터가 생성되거나, 엑터의 속성이나 트랜스폼 정보가 변경될 때 호출되는 함수
void AEPItemBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // 에디터 전용
#if WITH_EDITORONLY_DATA
    if (PreviewMeshComp && PickupMesh)
    {
        // 변수(PickupMesh)가 바뀌면 -> 컴포넌트(PreviewMeshComp)에 즉시 반영
        PreviewMeshComp->SetStaticMesh(PickupMesh);

        // 크기 적용
        PreviewMeshComp->SetRelativeScale3D(ItemScale);
    }
#endif
}

void AEPItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

