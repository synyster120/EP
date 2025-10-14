// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/EPBTTask_PlayMontageFromBB.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EPCombatCharacterBase.h"

const FName UEPBTTask_PlayMontageFromBB::IsHitKey(TEXT("IsHit"));

UEPBTTask_PlayMontageFromBB::UEPBTTask_PlayMontageFromBB()
{
	bNotifyTick = false;
	bNotifyTaskFinished = true;
	NodeName = TEXT("PlayMontage From BlackBoard");
}

EBTNodeResult::Type UEPBTTask_PlayMontageFromBB::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	MyOwnerComp = OwnerComp;
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AEPCombatCharacterBase* OwnerCharacter = Cast<AEPCombatCharacterBase>(AIController->GetPawn());
	if (!AIController || !BlackboardComp || !OwnerCharacter) return EBTNodeResult::Failed;

	UAnimMontage* MontageToPlay = Cast<UAnimMontage>(BlackboardComp->GetValueAsObject(MontageToPlayKey.SelectedKeyName));
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!MontageToPlay || !AnimInstance) return EBTNodeResult::Failed;

	// 바인딩
	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &UEPBTTask_PlayMontageFromBB::OnMontageEnded);

	// 플레이 및 바인딩 연결
	AnimInstance->Montage_Play(MontageToPlay);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

	return EBTNodeResult::InProgress; // 진행중
}

// 이 태스크가 외부 요인(데코레이터의 Abort 등)에 의해 중단될 때 호출
EBTNodeResult::Type UEPBTTask_PlayMontageFromBB::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	ACharacter* MyCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	UAnimInstance* AnimInstance = MyCharacter ? MyCharacter->GetMesh()->GetAnimInstance() : nullptr;

	// 현재 재생 중인 몽타주가 있다면 즉시 멈춰서 깨끗하게 정리
	if (AnimInstance && AnimInstance->Montage_IsPlaying(nullptr))
	{
		AnimInstance->Montage_Stop(0.1f);
	}

	// MyOwnerComp를 초기화하여 OnMontageEnded가 실수로 호출되는 것을 방지
	MyOwnerComp = nullptr;

	// 혹시 모를 바인딩 클리어 로직 추가 필요

	return EBTNodeResult::Succeeded;
}

void UEPBTTask_PlayMontageFromBB::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 태스크 성공 알림
	if (MyOwnerComp && !bInterrupted)
	{
		FinishLatentTask(*MyOwnerComp, EBTNodeResult::Succeeded);
	}
}