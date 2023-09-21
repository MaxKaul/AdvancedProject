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
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager, !NullcheckDependencies"));
			return;;
	}

	if (!InitAllManager())
		UE_LOG(LogTemp, Error, TEXT("SAVEGAME MANAGER, ERROR LOADING DATA"));


	TArray<FResourceTransactionTicket> transactioncall;
	// transactionreturn sollte 0 geld u. 10 ERI_Iron zurück bekommen
	transactioncall.Add( FResourceTransactionTicket(10, 20, EResourceIdent::ERI_Iron));
	// transactionreturn sollte 10 geld u. 1 ERI_Gold zurück bekommen
	transactioncall.Add( FResourceTransactionTicket(1, 10, EResourceIdent::ERI_Gold));
	// transactionreturn sollte 20 geld u. 0 ERI_Wheat zurück bekommen
	transactioncall.Add( FResourceTransactionTicket(100, 20, EResourceIdent::ERI_Wheat));


	TArray<FResourceTransactionTicket> transactionreturn = spawnedMarketManager->BuyResources(transactioncall);

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
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager !marketStall"));

	return status;
}

// Nur schonmal angelegt
bool ASaveGameManager::SaveGameData()
{
	bool status = false;

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Error, TEXT("SAVEGAME MANAGER, ERROR SAVING DATA"))
		return false;
	}

	spawnedMarketManager->GetManagerSaveData();

	return status;
}
