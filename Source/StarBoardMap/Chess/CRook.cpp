// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CRook.h"

void ACRook::BeginPlay()
{
    Name = FName("Rook");
    UnitTurnType = 3;
    UnitType = 5;
    Super::BeginPlay();
}

void ACRook::Attack()
{
}
