// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CBishop.h"

void ACBishop::BeginPlay()
{
    Name = FName("Bishop");
    UnitTurnType = 1;
    UnitType = 3;
    Super::BeginPlay();
}

void ACBishop::Attack()
{
}
