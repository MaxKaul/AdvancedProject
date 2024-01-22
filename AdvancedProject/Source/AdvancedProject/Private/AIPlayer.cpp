// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPlayer.h"

#include "AIStates.h"

AAIPlayer::AAIPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	states = CreateDefaultSubobject<UAIStates>("AIStates");
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

void AAIPlayer::State_Wait()
{
}

void AAIPlayer::State_BuyResources()
{
}

void AAIPlayer::State_SellResources()
{
}

void AAIPlayer::State_TransportResources()
{
}

void AAIPlayer::State_BuildSite()
{
}

void AAIPlayer::State_FireWorker()
{
}

void AAIPlayer::State_HireWorker()
{
}

void AAIPlayer::State_AssignWorker()
{
}

void AAIPlayer::State_UnassignWorker()
{
}
