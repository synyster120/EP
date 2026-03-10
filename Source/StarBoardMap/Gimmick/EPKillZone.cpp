

#include "Gimmick/EPKillZone.h"
#include "Components/BoxComponent.h"
#include "Characters/EPPlayerCharacter.h"

AEPKillZone::AEPKillZone()
{
	PrimaryActorTick.bCanEverTick = false;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;

    // Trigger 프로파일 (물리적 충돌 없이 겹침만 감지)
    CollisionBox->SetCollisionProfileName(TEXT("Trigger"));

    // 기본 크기 설정
    CollisionBox->SetBoxExtent(FVector(500.f, 500.f, 50.f));
}

void AEPKillZone::BeginPlay()
{
	Super::BeginPlay();

    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEPKillZone::OnOverlapBegin);
}

void AEPKillZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 플레이어인지 확인
    if (OtherActor)
    {
        AEPPlayerCharacter* PlayerCharacter = Cast<AEPPlayerCharacter>(OtherActor);
        if (PlayerCharacter)
        {
            // 리스폰 처리 요청
            PlayerCharacter->StartRespawnSequence();
        }
    }
}
