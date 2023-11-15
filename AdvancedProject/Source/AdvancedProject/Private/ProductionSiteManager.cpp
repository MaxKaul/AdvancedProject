// Fill out your copyright notice in the Description page of Project Settings.


#include "ProductionSiteManager.h"

#include "Productionsite.h"
#include "BuildingSite.h"
#include "Builder.h"
#include "WorkerWorldManager.h"

// Sets default values
UProductionSiteManager::UProductionSiteManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void UProductionSiteManager::BeginPlay()
{
	Super::BeginPlay();
}

void UProductionSiteManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("UProductionSiteManager, !NullcheckDependencies()"))
			return;
	}
}

void UProductionSiteManager::InitProductionSiteManager(APlayerBase* _managerOwner, FProductionSiteManagerSaveData _saveData, AMarketManager* _marketManager, AWorkerWorldManager* _workerWorldManager)
{
	world = GetWorld();
	managerOwner = _managerOwner;
	marketManager = _marketManager;
	workerWorldManager = _workerWorldManager;

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("UProductionSiteManager, !NullcheckDependencies()"))
			return;
	}

	InitAllProductionSites(_saveData);
}

void UProductionSiteManager::InitAllProductionSites(FProductionSiteManagerSaveData _saveData)
{
	// -> Hier nehme ich die save data entgegen nehmen

	TArray<FProductionSiteSaveData> allsavedsites = _saveData.GetSaveData();

	if (allsavedsites.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UProductionSiteManager, Init from save data was called, but no save data was found"))
		return;
	}


	for (FProductionSiteSaveData sitedata : allsavedsites)
	{
		ABuildingSite* savedbuildingsite = sitedata.GetSavedBuildingSite();

		FVector spawnpos = savedbuildingsite->GetActorLocation();
		FRotator spawnrot = savedbuildingsite->GetActorRotation();

		// Debug weil die gebäude meshes noch angepasst werden müssen
		spawnpos.Z -= 300.f;

		AProductionsite* spawnedsite = Cast<AProductionsite>(world->SpawnActor(productionSiteClass, &spawnpos, &spawnrot));

		FPS_OverloadFuncs* ps_overloadfuncs;
		ps_overloadfuncs = new FPS_OverloadFuncs(spawnedsite);

		ps_overloadfuncs->InitProductionSite(sitedata, marketManager, managerOwner, ps_overloadfuncs, this);

		allProductionSites.Add(spawnedsite);
		
		savedbuildingsite->SetBuildStatus(true);
	}
}

void UProductionSiteManager::SubscribeProductionsite(AProductionsite* _newProductionSite)
{
	if(!allProductionSites.IsEmpty() && allProductionSites.Contains(_newProductionSite))
	{
		UE_LOG(LogTemp,Warning,TEXT("UProductionSiteManager, allProductionSites.Contains(_newProductionSite)"))
		return;
	}
	
	allProductionSites.Add(_newProductionSite);
}

FProductionSiteManagerSaveData UProductionSiteManager::GetProductionSiteManagerSaveData()
{
	TArray<FProductionSiteSaveData> allpssavedata;
	
	for(AProductionsite* site : allProductionSites)
	{
		FProductionSiteSaveData savedata = site->GetProductionSiteSaveData();

		allpssavedata.Add(savedata);


		// 	Geht net weril kein linksseiter == operand für das struct geht (nehm ich an)
		//if (!allpssavedata.Contains(savedata))
		//	allpssavedata.Add(savedata);
		//else
		//	UE_LOG(LogTemp, Warning, TEXT("UProductionSiteManager, Production Site save data duplicate"))
	}
	
	FProductionSiteManagerSaveData savedata =
	{
		allpssavedata
	};

	return savedata;
}

void UProductionSiteManager::SubscribeWorkerToLocalPool(AWorker* _toSub, bool _subFromSite)
{
	if (!subscribedWorker_UnasignedPool.Contains(_toSub))
	{
		subscribedWorker_UnasignedPool.Add(_toSub);
		_toSub->SetEmployementStatus(EWorkerStatus::WS_Unassigned);

		if (!_subFromSite)
			workerWorldManager->UnsubWorkerFromUnemploymentPool(_toSub);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, subscribedWorker.Contains(_toSub)"));

	if (!subscribedWorker_HiredPool.Contains(_toSub))
		subscribedWorker_HiredPool.Add(_toSub);
}

void UProductionSiteManager::UnsubscribeWorkerToProductionSite(AWorker* _toUnsub)
{
	if (subscribedWorker_UnasignedPool.Contains(_toUnsub))
		subscribedWorker_UnasignedPool.Remove(_toUnsub);
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, !subscribedWorker.Contains(_toUnsub)"))
}

void UProductionSiteManager::UnsubscribeWorkerToWorldPool(AWorker* _toUnsub)
{
	if (subscribedWorker_HiredPool.Contains(_toUnsub))
	{
		workerWorldManager->SubWorkerToUnemploymentPool(_toUnsub);
		_toUnsub->SetEmployementStatus(EWorkerStatus::WS_Unemployed);
		subscribedWorker_HiredPool.Remove(_toUnsub);
		subscribedWorker_UnasignedPool.Remove(_toUnsub);

		for(AProductionsite* site : allProductionSites)
		{
			if (site->GetLocalProdSiteID() == _toUnsub->GetAssignedProdSiteID())
				site->UnsubscribeWorker(_toUnsub, false);
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, !subscribedWorker.Contains(_toUnsub)"))
}


bool UProductionSiteManager::NullcheckDependencies()
{
	bool status = true;

	if(!productionSiteClass)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("UProductionSiteManager !productionSiteClass"));
	}

	if (!world)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("UProductionSiteManager !world"));
	}

	if (!managerOwner)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("UProductionSiteManager !managerOwner"));
	}

	if (!marketManager)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("UProductionSiteManager !marketManager"));
	}

	return status;
}
