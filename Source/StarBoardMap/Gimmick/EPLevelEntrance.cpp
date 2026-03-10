

#include "Gimmick/EPLevelEntrance.h"
#include "Components/ArrowComponent.h"

AEPLevelEntrance::AEPLevelEntrance()
{
	PrimaryActorTick.bCanEverTick = false;

    // 루트 컴포넌트 생성 및 설정
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    RootComponent = RootSceneComponent;

    // 화살표 컴포넌트 생성 및 루트에 부착
    SpawnDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnDirection"));
    SpawnDirection->SetupAttachment(RootComponent);

    // 화살표 색상(Green), 크기(1.5) 변경
    SpawnDirection->ArrowColor = FColor::Green;
    SpawnDirection->ArrowSize = 1.5f;
}

