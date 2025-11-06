// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess/CPawn.h"

void ACPawn::BeginPlay()
{
    Name = FName("Pawn");
    UnitType = 6;
    MovingTurn = false;
    Super::BeginPlay();
}

void ACPawn::Attack()
{
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}
