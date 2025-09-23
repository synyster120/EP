
#include "Characters/EPCharacterBase.h"


AEPCharacterBase::AEPCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false;

}

void AEPCharacterBase::PlayAnimationByTag(FGameplayTag NewTag)
{
	// 태그에 맞는 몽타주 검색 후 Play

}

void AEPCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEPCharacterBase::InitializeCharacterData()
{
	// 초기화 로직
}
