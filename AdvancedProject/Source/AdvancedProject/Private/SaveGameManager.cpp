// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameManager.h"

#include "ASPSaveGame.h"
#include "ProductionSiteManager.h"
#include "MarketManager.h"
#include "Productionsite.h"
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
	bool status = true;

	FVector pos = FVector(0.f);

	spawnedMarketManager = Cast<AMarketManager>(world->SpawnActor(marketManagerClass, &pos));

	if (!spawnedMarketManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager, !spawnedMarketManager"));
		status = false;
	}

	if (marketManagerSaveData.IsSet())
		spawnedMarketManager->InitMarketManager(marketManagerSaveData.GetValue());
	else
		spawnedMarketManager->InitMarketManager();


	spawnedWorkerWorldManager = Cast<AWorkerWorldManager>(world->SpawnActor(workerWorldManagerClass, &pos));

	if (!spawnedMarketManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager, !spawnedMarketManager"));
		return false;
	}
	else
		status = true;

	if (workerWorldManagerSaveData.IsSet())
		spawnedWorkerWorldManager->InitWorkerWorldManager(workerWorldManagerSaveData.GetValue());
	else
		spawnedWorkerWorldManager->InitWorkerWorldManager();


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

	UASPSaveGame* newsave = Cast<UASPSaveGame>(UGameplayStatics::CreateSaveGameObject(UASPSaveGame::StaticClass()));
	newsave->InitSaveGame(marketmanagersavedata, workerworldmanagersavedata);

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

		marketManagerSaveData = loadedsavegame->GetSavedMarketManagerData();
		workerWorldManagerSaveData = loadedsavegame->GetSavedWorkerWorldManagerSaveData();
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