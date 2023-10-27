// Fill out your copyright notice in the Description page of Project Settings.


#include "ASPSaveGame.h"
#include "MarketManager.h"
#include "PlayerBase.h"

UASPSaveGame::UASPSaveGame()
{

}

void UASPSaveGame::InitSaveGame(FMarketManagerSaveData _marketManagerSaveData, FWorkerWorldManagerSaveData _workerWorldManagerSaveData, TArray<FPlayerSaveData> _allPlayerSaveData)
{
	marketManagerSaveData = _marketManagerSaveData;
	workerWorldManagerSaveData = _workerWorldManagerSaveData;
	allPlayerSaveData = _allPlayerSaveData;
}
