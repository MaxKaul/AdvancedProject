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

void UAIStates::State_Wait()
{
	UE_LOG(LogTemp, Warning, TEXT("State_Wait"));
}

bool UAIStates::State_BuyResources()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_BuyResources"));

	return status;
}

bool UAIStates::State_SellResources()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_SellResources"));

	return status;
}

bool UAIStates::State_TransportResources()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_TransportResources"));

	return status;
}

bool UAIStates::State_BuildSite()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_BuildSite"));

	return status;
}

bool UAIStates::State_FireWorker()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_FireWorker"));

	return status;
}

bool UAIStates::State_HireWorker()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_HireWorker"));

	return status;
}

bool UAIStates::State_AssignWorker()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_AssignWorker"));

	return status;
}

bool UAIStates::State_UnassignWorker()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_UnassignWorker"));

	return status;
}