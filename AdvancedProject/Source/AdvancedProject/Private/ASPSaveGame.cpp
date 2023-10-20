// Fill out your copyright notice in the Description page of Project Settings.


#include "ASPSaveGame.h"

#include "MarketManager.h"
#include "Kismet/GameplayStatics.h"

UASPSaveGame::UASPSaveGame()
{

}

void UASPSaveGame::InitSaveGame(FMarketManagerSaveData _marketManagerSaveData, FWorkerWorldManagerSaveData _workerWorldManagerSaveData)
{
	marketManagerSaveData = _marketManagerSaveData;
	workerWorldManagerSaveData = _workerWorldManagerSaveData;
}
