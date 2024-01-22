// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPlayer.h"

AAIPlayer::AAIPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAIPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAIPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}