// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkerMoodManager.h"

// Sets default values for this component's properties
UWorkerMoodManager::UWorkerMoodManager()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UWorkerMoodManager::BeginPlay()
{
	Super::BeginPlay();

}


void UWorkerMoodManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWorkerMoodManager::InitWorkerMoodManager(AWorker* _owner, FWorkerMoodManagerSaveData _saveData)
{
	managerOwner = _owner;
}