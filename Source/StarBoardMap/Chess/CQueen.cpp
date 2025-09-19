// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CQueen.h"

void ACQueen::BeginPlay()
{
    Name = FName("Queen");
    MovingTurn = true;
    UnitType = 2;
    Super::BeginPlay();
}
