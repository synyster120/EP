// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EPHealthUnitWidget.h"

void UEPHealthUnitWidget::UpdateState(bool bIsFull)
{
	// 여기서 로직이 더 필요하면 작성 (예: 소리 재생 등)
	
	// BP 쪽에 신호를 보냄
	OnStateChanged(bIsFull);
}


