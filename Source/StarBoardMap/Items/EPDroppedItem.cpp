
#include "Items/EPDroppedItem.h"
#include "Components/SphereComponent.h"
#include "Items/EPItemBase.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Interfaces/EPLootListenerInterface.h"

AEPDroppedItem::AEPDroppedItem()
{
	PrimaryActorTick.bCanEverTick = true;

    // 컴포넌트 설정 (물리 켜기 등)
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
    MeshComp->SetSimulatePhysics(true); // 물리 필수
    MeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));

    // collision은 simple Collision 사용

    // 위젯 컴포넌트 생성 및 부착
    PickupWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
    PickupWidgetComp->SetupAttachment(RootComponent);


    // 위젯 공간 설정 (Screen vs World)
    // Screen: 항상 카메라를 정면으로 바라보고, 거리에 상관없이 일정한 크기로 보임 (UI용)
    PickupWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

    // 위치 기본값 설정 (InitializeDrop 함수에서 재설정)
    PickupWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));

    // 크기 자동 조절
    // 위젯 블루프린트의 Desired Size에 맞춰서 크기를 자동으로 잡음
    PickupWidgetComp->SetDrawAtDesiredSize(true);

    // 기본적으론 안보이게 꺼둠
    PickupWidgetComp->SetVisibility(false);

    // 충돌 끄기 (UI가 캐릭터 이동을 막거나, 라인트레이스에 걸리면 안 됨)
    PickupWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// 아이템 드랍
void AEPDroppedItem::InitializeDrop(TSubclassOf<AActor> ItemClass, int32 Quantity)
{
    OriginalItemClass = ItemClass;

    // 액터를 스폰하지 않고도, 그 클래스의 기본 설정값(변수)을 읽어올 수 있음 - CDO (Class Default Object)
    if (ItemClass)
    {
        // ItemClass가 AEPItemBase를 상속받았다고 가정
        AEPItemBase* DefaultItem = Cast<AEPItemBase>(ItemClass->GetDefaultObject());
        if (DefaultItem && DefaultItem->PickupMesh)
        {
            // 원본 아이템에 설정된 메시를 내 메시로 입힘
            MeshComp->SetStaticMesh(DefaultItem->PickupMesh);

            // 크기 조절
            MeshComp->SetWorldScale3D(DefaultItem->ItemScale);

            // 무게 조절
            MeshComp->SetMassOverrideInKg(NAME_None, DefaultItem->ItemMass, true);

            // 너무 잘 굴러가는 걸 방지하기 위해 댐핑(저항)
            MeshComp->SetLinearDamping(0.5f);  // 공기 저항 (덜 날아감)
            MeshComp->SetAngularDamping(2.0f); // 회전 저항 (덜 굴러감)

            // 위젯 위치 조정
            if (PickupWidgetComp)
            {
                PickupWidgetComp->SetRelativeLocation(DefaultItem->WidgetOffset); // ItemBase의 위젯오프셋 값으로 설정
                
                // 이름 세팅
                if (PromptWidget)
                {
                    PromptWidget->SetItemName(DefaultItem->DisplayName);
                }
            }
        }
    }

    bStart = true; // 준비 완료
}

void AEPDroppedItem::BeginPlay()
{
	Super::BeginPlay();

    // 테스트용 드랍 
    // 람다에 this 를 넣으면 객체가 파괴되었을 때 "OriginalItemClass" 같은 객체 접근에 크래시 발생
    FTimerHandle SpawnDelayHandle; 
    TWeakObjectPtr<AEPDroppedItem> WeakThis(this); // 나(this)를 가리키는 약한 포인터 객체 생성 (this 대신)
    GetWorld()->GetTimerManager().SetTimer(
        SpawnDelayHandle,
        FTimerDelegate::CreateLambda([WeakThis]()
            {
                // 실행 시점에 내가 살아있는지 확인 (IsValid 가능)
                if (AEPDroppedItem* StrongThis = WeakThis.Get())
                {
                    // 살아있다면 StrongThis 포인터를 통해 멤버에 접근
                    if (StrongThis->OriginalItemClass && !StrongThis->bStart)
                    {
                        StrongThis->InitializeDrop(StrongThis->OriginalItemClass, 1);
                    }
                }
                else
                {
                    // 이미 파괴된 상태라면 아무것도 하지 않고 무시
                    UE_LOG(LogTemp, Log, TEXT("DroppedItem was destroyed before timer finished."));
                }
            }),
        3.5f,
        false
    );

    // 컴포넌트 안에 있는 위젯 객체를 가져와서, 내 전용 클래스로 형변환(Cast)
    if (PickupWidgetComp)
    {
        // 위젯 검사 및 저장
        PromptWidget = Cast<UEPInteractionPromptWidget>(PickupWidgetComp->GetUserWidgetObject());

        // 이름 세팅
        if (PromptWidget)
        {
            PromptWidget->SetItemName(FText::FromString(TEXT("Default Item")));
        }
    }
}

// overlap Actors 에게 죽음 알림(유언)
void AEPDroppedItem::NotifyActorsBeforeDestruction()
{
    // 현재 나와 겹쳐있는 모든 액터를 가져옴 (물리 엔진 활용)
    TArray<AActor*> OverlappingActors;
    GetOverlappingActors(OverlappingActors); // AActor 클래스에 내장된 표준 함수 (overlap된 actors 찾아줌)

    // 하나씩 순회하며 "나 사라진다"고 알림
    for (AActor* Actor : OverlappingActors)
    {
        // 인터페이스(유언 듣는 귀) 상속 받았는지 판단
        if (Actor && Actor->Implements<UEPLootListenerInterface>())
        {
            IEPLootListenerInterface::Execute_OnItemRemoved(Actor, this);
        }
    }
}

// 삭제 처리 함수
void AEPDroppedItem::DestroyItem()
{
    // 죽기전 overlap Actors 에게 죽음 알림(유언)
    NotifyActorsBeforeDestruction();

    Destroy(); // 실제 삭제
}

void AEPDroppedItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// UI 보이기
void AEPDroppedItem::ShowInteractionUI()
{
    if (PickupWidgetComp) PickupWidgetComp->SetVisibility(true);
}

// UI 숨기기
void AEPDroppedItem::HideInteractionUI()
{
    if (PickupWidgetComp) PickupWidgetComp->SetVisibility(false);
}

