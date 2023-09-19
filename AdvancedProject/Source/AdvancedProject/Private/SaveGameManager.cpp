// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameManager.h"
#include "MarketManager.h"

ASaveGameManager::ASaveGameManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ASaveGameManager::BeginPlay()
{
	Super::BeginPlay();

	world = GetWorld();

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager, !NullcheckDependencies"))
			return;;
	}

	if(!InitAllManager())
		UE_LOG(LogTemp, Error,TEXT("SAVEGAME MANAGER, ERROR LOADING DATA"))
}

void ASaveGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ASaveGameManager::InitAllManager()
{
	bool status = false;

	FVector pos = FVector(0.f);

	spawnedMarketManager = Cast<AMarketManager>(world->SpawnActor(marketManagerClass, &pos));

	if (!spawnedMarketManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager, !spawnedMarketManager"))
			return false;
	}
	else
		status = true;

	if (marketManagerSaveData.IsSet())
		spawnedMarketManager->InitMarketManager(marketManagerSaveData.GetValue());
	else
		spawnedMarketManager->InitMarketManager(false);

	return status;
}


bool ASaveGameManager::NullcheckDependencies()
{
	bool status = false;

	if (marketManagerClass)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager !marketStall"));

	return status;
}
