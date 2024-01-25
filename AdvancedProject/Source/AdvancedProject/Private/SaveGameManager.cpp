 // Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameManager.h"

#include "ASPSaveGame.h"
#include "PlayerBase.h"
#include "MarketManager.h"
#include "Productionsite.h"
#include "SpectatorPlayer.h"
#include "AdvancedProject/AdvancedProjectCharacter.h"
#include "AdvancedProject/AdvancedProjectPlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"

ASaveGameManager::ASaveGameManager()
{
	PrimaryActorTick.bCanEverTick = true;
	maxPlayerAmount = 1;


	defaultStartPos = { -9264.f, 4692.f, -727.f };
	defaultStartRot = {  };
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

	status = InitMarketManager();
	status = SpawnWorkerWorldManager();
	status = InitPlayer();

	return status;
}

 bool ASaveGameManager::InitMarketManager()
 {
	 //----------------------------------------------------------------------------------------------------------------------------//
	 //Spawning and Init of the MarketManager and Test Player//
	 //----------------------------------------------------------------------------------------------------------------------------//

	 bool status = true;
	 FVector pos = FVector(0.f);

	 spawnedMarketManager = Cast<AMarketManager>(world->SpawnActor(marketManagerClass, &pos));

	 if (!spawnedMarketManager)
	 {
		 UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager, !spawnedMarketManager"));
		 return status = false;
	 }

	 FMM_OverloadFuncs* mm_overloadfuncs;
	 mm_overloadfuncs = new FMM_OverloadFuncs(spawnedMarketManager);

	 if (saveManagerOptionals.marketManagerSaveData.IsSet())
		 mm_overloadfuncs->InitMarketManager(saveManagerOptionals.marketManagerSaveData.GetValue());
	 else
		 mm_overloadfuncs->InitMarketManager();

	 return status;
 }

 bool ASaveGameManager::InitPlayer()
 {
	 //----------------------------------------------------------------------------------------------------------------------------//
	 //Spawning and Init of all Player Entities//
	 //----------------------------------------------------------------------------------------------------------------------------//

	 bool status = true;


	 if (saveManagerOptionals.allPlayerSaveData.IsSet())
	 {
		 for(FPlayerSaveData data : saveManagerOptionals.allPlayerSaveData.GetValue())
		 {
			 if(data.GetIdent_S() != EPlayerIdent::PI_Player_1)
			 {
				APlayerBase* aiplayer = world->SpawnActor<APlayerBase>(aiPlayer, defaultStartPos, defaultStartRot);
				aiplayer->InitPlayerStart(data, spawnedMarketManager, spawnedWorkerWorldManager, allBuildingSites);
				allPlayer.Add(aiplayer);
			 }
			 else
			 {
				 APlayerBase* player = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(world, 0));
				 player->InitPlayerStart(data, spawnedMarketManager, spawnedWorkerWorldManager, allBuildingSites);
				 allPlayer.Add(player);
			 }
		 }

	 	 for(APlayerBase* player : allPlayer)
	 	 {
			for(AProductionsite* site : player->GetProductionSiteManager()->GetAllProductionSites())
			{
				allSavedProductionsites.Add(site);
			}
	 	 }
	 
	 	 InitWorkerWorldManager();
	 }
	 else
	 { 
	 	 for (size_t i = 0; i < maxPlayerAmount; i++)
	 	 {
			 EPlayerIdent ident = EPlayerIdent::PI_DEFAULT;

			 if (i == 0)
				 ident = EPlayerIdent::PI_Player_1;
			 else if (i == 1)
				 ident = EPlayerIdent::PI_Player_2;
			 else if (i == 2)
				 ident = EPlayerIdent::PI_Player_3;
			 else if (i == 3)
				 ident = EPlayerIdent::PI_Player_4;
			 else
				 ident = EPlayerIdent::PI_DEFAULT;


			 if (ident == EPlayerIdent::PI_Player_1 )
				 InitFreshPlayer(Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(world, 0)), ident);
			else
				 InitFreshPlayer(world->SpawnActor<APlayerBase>(aiPlayer, defaultStartPos, defaultStartRot), ident);
	 	 }
	 }

	 return status;
 }

 void ASaveGameManager::InitFreshPlayer(APlayerBase* _newplayer, EPlayerIdent _ident)
 {
	 FPlayerSaveData initsave;

	 SubscribePlayer(_newplayer);

	 // Leere Prodsite save data weil dieser ja keine werte besitzen wird wenn wir neu starten
	 initsave.InitPlayerSaveData(defaultStartPos, defaultStartRot, FProductionSiteManagerSaveData(), _ident, FTransportManagerSaveData(), 5000.f);
	 _newplayer->InitPlayerStart(initsave, spawnedMarketManager, spawnedWorkerWorldManager, allBuildingSites);
 }


 bool ASaveGameManager::SpawnWorkerWorldManager()
 {
	 bool status = true;
	 FVector pos = FVector(0.f);

	 spawnedWorkerWorldManager = Cast<AWorkerWorldManager>(world->SpawnActor(workerWorldManagerClass, &pos));

	 if (saveManagerOptionals.workerWorldManagerSaveData.IsSet())
		 return status;

	 FWWM_OverloadFuncs* wwm_overloadfuncs;
	 wwm_overloadfuncs = new FWWM_OverloadFuncs(spawnedWorkerWorldManager);

	// Der Worker World Manager wird Init nachdem alle spieler gespawned sind, davor wird er nur gespawned
	wwm_overloadfuncs->InitWorkerWorldManager(this, spawnedMarketManager);

	 return status;
 }

 bool ASaveGameManager::InitWorkerWorldManager()
 {
	 bool status = true;

	 if (!saveManagerOptionals.workerWorldManagerSaveData.IsSet())
		 return status;

	 FWWM_OverloadFuncs* wwm_overloadfuncs;
	 wwm_overloadfuncs = new FWWM_OverloadFuncs(spawnedWorkerWorldManager);
	 wwm_overloadfuncs->InitWorkerWorldManager(saveManagerOptionals.workerWorldManagerSaveData.GetValue(), allSavedProductionsites,this, spawnedMarketManager);

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

	if (!aiPlayer)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager !aiPlayer"));
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
