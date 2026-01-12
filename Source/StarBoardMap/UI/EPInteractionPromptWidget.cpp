// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EPInteractionPromptWidget.h"

void UEPInteractionPromptWidget::SetItemName(FText NewName)
{
    if (Txt_Name)
    {
        Txt_Name->SetText(NewName);
    }
}
