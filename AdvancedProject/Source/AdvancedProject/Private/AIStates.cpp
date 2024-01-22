// Fill out your copyright notice in the Description page of Project Settings.


#include "AIStates.h"

UAIStates::UAIStates()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAIStates::BeginPlay()
{
	Super::BeginPlay();


}

void UAIStates::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

