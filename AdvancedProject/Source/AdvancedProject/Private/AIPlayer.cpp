// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPlayer.h"

// Sets default values
AAIPlayer::AAIPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAIPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

