

#include "Gimmick/EPCheckpoint.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Characters/EPPlayerCharacter.h"
#include "Core/Subsystems/EPRespawnSubsystem.h"

AEPCheckpoint::AEPCheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;

    // 박스 콜리전 초기화 및 루트 컴포넌트로 설정
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;
    CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
    CollisionBox->SetBoxExtent(FVector(100.f, 100.f, 50.f)); // 기본 크기 설정

    // 화살표 컴포넌트 초기화 및 박스 콜리전에 부착
    ArrowDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowDirection"));
    ArrowDirection->SetupAttachment(RootComponent);
}

void AEPCheckpoint::BeginPlay()
{
	Super::BeginPlay();

	// 오버랩 이벤트 바인딩
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEPCheckpoint::OnOverlapBegin);
}

void AEPCheckpoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 닿은 액터가 캐릭터 클래스인지 확인 (몬스터나 투사체 무시)
    if (OtherActor && OtherActor->IsA(AEPPlayerCharacter::StaticClass()))
    {
        if (UGameInstance* GameInstance = GetGameInstance())
        {
            if (UEPRespawnSubsystem* RespawnSubsystem = GameInstance->GetSubsystem<UEPRespawnSubsystem>())
            {
                // 화살표의 현재 위치와 회전값을 서브시스템에 전달 (체크포인트 위치 갱신)
                RespawnSubsystem->SetRespawnPoint(ArrowDirection->GetComponentTransform());

            }
        }
    }
}

