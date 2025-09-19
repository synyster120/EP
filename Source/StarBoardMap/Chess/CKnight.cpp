// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CKnight.h"

void ACKnight::BeginPlay()
{
    Name = FName("Knight");
    UnitTurnType = 2;
    UnitType = 4;
    Super::BeginPlay();
}

void ACKnight::Attack()
{
}
