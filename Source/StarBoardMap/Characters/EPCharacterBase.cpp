
#include "Characters/EPCharacterBase.h"

AEPCharacterBase::AEPCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false;

}

void AEPCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEPCharacterBase::InitializeCharacterData()
{
	// 초기화 로직
}
