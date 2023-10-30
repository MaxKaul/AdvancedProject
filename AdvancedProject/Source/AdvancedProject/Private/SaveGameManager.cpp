 // Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameManager.h"

#include "ASPSaveGame.h"
#include "PlayerBase.h"
#include "MarketManager.h"
#include "Productionsite.h"
#include "AdvancedProject/AdvancedProjectCharacter.h"
#include "Kismet/GameplayStatics.h"

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


	/*


    //MUSTER: Zum Resourcen kaufen/verkaufen/
	//// Beim Kaufen steht unser exchange capital für das geld welches wir mitgeben, dies ist der MaxBuyPricePerResource * BuyAmount
	//// Erstmal als flicken, da kann noch was besseres ran
	//TOptional<float> nomaxsellprice;
	//TArray<FResourceTransactionTicket> transactioncall =
	//{
	//	FResourceTransactionTicket(10, 100, EResourceIdent::ERI_Iron, 1,nomaxsellprice),
	//	FResourceTransactionTicket(1, 10, EResourceIdent::ERI_Gold, 10,nomaxsellprice),
	//	FResourceTransactionTicket(100, 1000, EResourceIdent::ERI_Wheat, 10,nomaxsellprice),
	//};
	//TArray<FResourceTransactionTicket> transactionreturn = spawnedMarketManager->BuyResources(transactioncall);

	// //Beim kaufen steht der _exchnagedCapital wert für das geld welches wir für die totale individuelle resource haben möchet
	// //-> Dies kann man sich merken wie; Wir kaufen mit total wert und der markt checkt individuelle resourcen und wir verkauffen für individualwert und der markt checkt für total
	//TOptional<float> nomaxbuyprice;
	//TArray<FResourceTransactionTicket> transactioncall =
	//{
	//	FResourceTransactionTicket(100, 0, EResourceIdent::ERI_Iron, nomaxbuyprice,3),
	//	FResourceTransactionTicket(10, 0, EResourceIdent::ERI_Gold, nomaxbuyprice,0),
	//	FResourceTransactionTicket(10, 0, EResourceIdent::ERI_Wheat, nomaxbuyprice,0),
	//};
	//TArray<FResourceTransactionTicket> transactionreturn = spawnedMarketManager->SellResources(transactioncall);

	//for (FResourceTransactionTicket ticket : transactionreturn)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("exchanged capital %f, returned resource %i"), ticket.exchangedCapital, ticket.resourceAmount);
	//}
 	 *
	 */
}

void ASaveGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager, !NullcheckDependencies"));
		return;;
	}
}

bool ASaveGameManager::InitAllManager()
{
	LoadGameData("save_0", 0);

	bool status = true;
	FVector pos = FVector(0.f);


	//----------------------------------------------------------------------------------------------------------------------------//
	//Spawning and Init of the MarketManager and Test Player//
	//----------------------------------------------------------------------------------------------------------------------------//

	spawnedMarketManager = Cast<AMarketManager>(world->SpawnActor(marketManagerClass, &pos));

	if (!spawnedMarketManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager, !spawnedMarketManager"));
		return status = false;
	}

	FMM_OverloadFuncs* mm_overloadfuncs;
	mm_overloadfuncs = new FMM_OverloadFuncs(spawnedMarketManager);

	// Nur zum testen, der player hat manuel gespawned zu werden
	APlayerBase* testplayer = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	SubscribePlayer(testplayer);

	if (saveManagerOptionals.marketManagerSaveData.IsSet())
		mm_overloadfuncs->InitMarketManager(saveManagerOptionals.marketManagerSaveData.GetValue());
	else
		mm_overloadfuncs->InitMarketManager();


	//----------------------------------------------------------------------------------------------------------------------------//
	//Spawning and Init of all Player Entities//
	//----------------------------------------------------------------------------------------------------------------------------//

	// Muss noch etwas geändert werden -> Manueller Spawn + der spieler braucht ein flag um ihn zu unterscheiden für den spawn
	if (saveManagerOptionals.allPlayerSaveData.IsSet())
		allPlayer[0]->InitPlayer(saveManagerOptionals.allPlayerSaveData.GetValue()[0], spawnedMarketManager);
	else
	{
		// Könnte soweit geändert werden das dies zum beispiel default start werte sein können
		FPlayerSaveData emptysave;

		FVector ttt = { -9264.f, 4692.f, -727.f };
		FRotator ddd = {  };

		emptysave.InitSaveData(ttt, ddd, FProductionSiteManagerSaveData());

		allPlayer[0]->InitPlayer(emptysave, spawnedMarketManager);
	}


	//----------------------------------------------------------------------------------------------------------------------------//
	//Spawning and Init of all Productionsites (will onloy spawn with savedata)//
	//----------------------------------------------------------------------------------------------------------------------------//

	TArray<AProductionsite*> allproductionsites;

	for(APlayerBase* player : allPlayer)
	{
		for (AProductionsite* site : player->GetProductionSiteManager()->GetAllProductionSites())
		{
			allproductionsites.Add(site);
		}
	}


	//----------------------------------------------------------------------------------------------------------------------------//
	//Spawning and Init of the WorkerWorldManager//
	//----------------------------------------------------------------------------------------------------------------------------//

	spawnedWorkerWorldManager = Cast<AWorkerWorldManager>(world->SpawnActor(workerWorldManagerClass, &pos));

	FWWM_OverloadFuncs* wwm_overloadfuncs;
	wwm_overloadfuncs = new FWWM_OverloadFuncs(spawnedWorkerWorldManager);

	if (saveManagerOptionals.workerWorldManagerSaveData.IsSet())
		wwm_overloadfuncs->InitWorkerWorldManager(saveManagerOptionals.workerWorldManagerSaveData.GetValue(), allproductionsites);
	else
		wwm_overloadfuncs->InitWorkerWorldManager();

	return status;
}

bool ASaveGameManager::SaveGameData()
{
	bool status = false;

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Error, TEXT("SAVEGAME MANAGER, ERROR SAVING DATA"))
		return false;
	}


	FString savedir = FPaths::ProjectSavedDir() + TEXT("SaveGames/");  

	IFileManager::Get().FindFiles(savedGames, *savedir, TEXT("*.sav")); 

	if (saveGameName == "")
		saveGameName = FString("save_") + FString::FromInt(savedGames.Num());


	FMarketManagerSaveData marketmanagersavedata = spawnedMarketManager->GetMarketManagerSaveData();
	FWorkerWorldManagerSaveData workerworldmanagersavedata = spawnedWorkerWorldManager->GetWorkerManagerSaveData();
	TArray<FPlayerSaveData> allplayersavedata;

	for(APlayerBase* player : allPlayer)
	{
		FPlayerSaveData newplayer;
		
		newplayer = player->GetPlayerSaveData();

		allplayersavedata.Add(newplayer);
	}

	UASPSaveGame* newsave = Cast<UASPSaveGame>(UGameplayStatics::CreateSaveGameObject(UASPSaveGame::StaticClass()));
	newsave->InitSaveGame(marketmanagersavedata, workerworldmanagersavedata, allplayersavedata);

	int saveslot = savedGames.Num() + 1;

	UGameplayStatics::SaveGameToSlot(newsave, saveGameName, saveslot);

	return status;
}

bool ASaveGameManager::LoadGameData(FString _saveGameName, int _saveGameSlot)
{
	bool status = false;

	UASPSaveGame* loadedsavegame = Cast<UASPSaveGame>(UGameplayStatics::CreateSaveGameObject(UASPSaveGame::StaticClass()));
	loadedsavegame = Cast<UASPSaveGame>(UGameplayStatics::LoadGameFromSlot(_saveGameName, _saveGameSlot));

	if (IsValid(loadedsavegame))
	{
		status = true;

		saveManagerOptionals.marketManagerSaveData = loadedsavegame->GetSavedMarketManagerData();
		saveManagerOptionals.workerWorldManagerSaveData = loadedsavegame->GetSavedWorkerWorldManagerSaveData();
		saveManagerOptionals.allPlayerSaveData = loadedsavegame->GetAllPlayerSaveData();
	}

	// -> Get all the Data

	return status;
}

void ASaveGameManager::SetSaveGameName(FString _name)
{
	saveGameName = _name;
}

bool ASaveGameManager::NullcheckDependencies()
{
	bool status = true;

	if (!marketManagerClass)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager !marketStall"));
	}

	if (!world)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager !world"));
	}

	if (!workerWorldManagerClass)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager !workerWorldManagerClass"));
	}

	return status;
}

void ASaveGameManager::SubscribePlayer(APlayerBase* _toSub)
{
	if (!allPlayer.Contains(_toSub))
		allPlayer.Add(_toSub);
}

void ASaveGameManager::UnsubscribePlayer(APlayerBase* _toUnsub)
{
	if (allPlayer.Contains(_toUnsub))
		allPlayer.Remove(_toUnsub);
}
