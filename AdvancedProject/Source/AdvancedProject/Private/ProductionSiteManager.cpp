// Fill out your copyright notice in the Description page of Project Settings.


#include "ProductionSiteManager.h"

#include "Productionsite.h"
#include "BuildingSite.h"
#include "Builder.h"
#include "PlayerBase.h"
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
	InitWorkerLists();
}

void UProductionSiteManager::InitAllProductionSites(FProductionSiteManagerSaveData _saveData)
{
	// -> Hier nehme ich die save data entgegen nehmen

	TArray<FProductionSiteSaveData> allsavedsites = _saveData.GetSaveData();

	if (allsavedsites.Num() <= 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("UProductionSiteManager, Init from save data was called, but no sites have been saved"))
		return;
	}


	for (FProductionSiteSaveData sitedata : allsavedsites)
	{
		ABuildingSite* savedbuildingsite = sitedata.GetSavedBuildingSite();

		FVector spawnpos = savedbuildingsite->GetActorLocation();
		FRotator spawnrot = savedbuildingsite->GetActorRotation();

		// Debug weil die geb�ude meshes noch angepasst werden m�ssen
		spawnpos.Z -= 300.f;

		AProductionsite* spawnedsite = Cast<AProductionsite>(world->SpawnActor(productionSiteClass, &spawnpos, &spawnrot));

		FPS_OverloadFuncs* ps_overloadfuncs;
		ps_overloadfuncs = new FPS_OverloadFuncs(spawnedsite);

		ps_overloadfuncs->InitProductionSite(sitedata, marketManager, managerOwner, ps_overloadfuncs, this);

		allProductionSites.Add(spawnedsite);
		
		savedbuildingsite->SetBuildStatus(true);
	}
}

void UProductionSiteManager::InitWorkerLists()
{
	subscribedWorker_UnasignedPool = workerWorldManager->GetWorker_UnassignedPool().FindRef(managerOwner->GetPlayerIdent()).GetWorkerArray();
	subscribedWorker_HiredPool = workerWorldManager->GetWorker_AssignedPool().FindRef(managerOwner->GetPlayerIdent()).GetWorkerArray();
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

AProductionsite* UProductionSiteManager::GetSiteByID(int _id)
{
	if(allProductionSites.Num() <= 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("AProductionsite, allProductionSites.Num() <= 0"))
		return nullptr;
	}

	AProductionsite* foundsite = allProductionSites[0];

	for(AProductionsite* site : allProductionSites)
	{
		if(site->GetLocalProdSiteID() == _id)
		{
			foundsite = site;
			break;
		}
	}

	return foundsite;
}

FProductionSiteManagerSaveData UProductionSiteManager::GetProductionSiteManagerSaveData()
{
	TArray<FProductionSiteSaveData> allpssavedata;
	
	for(AProductionsite* site : allProductionSites)
	{
		FProductionSiteSaveData savedata = site->GetProductionSiteSaveData();

		allpssavedata.Add(savedata);


		// 	Geht net weril kein linksseiter == operand f�r das struct geht (nehm ich an)
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

void UProductionSiteManager::SubscribeWorkerToLocalPool(AWorker* _toSub, AProductionsite* _siteRef, bool _fromWorld)
{
	if (!subscribedWorker_UnasignedPool.Contains(_toSub))
	{
		subscribedWorker_UnasignedPool.Add(_toSub);

		if (!_fromWorld)
			_siteRef->UnsubscribeWorker(_toSub, true);
		else if (_fromWorld)
			workerWorldManager->UpdateWorkerStatus(_toSub, managerOwner->GetPlayerIdent());

		_toSub->SetWorkerState(EWorkerStatus::WS_Unassigned);
		WorkerWorldManagerUpdate(_toSub);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, subscribedWorker.Contains(_toSub)"));

	if (!subscribedWorker_HiredPool.Contains(_toSub))
		subscribedWorker_HiredPool.Add(_toSub);
}

void UProductionSiteManager::UnsubscribeWorkerToProductionSite(AWorker* _toUnsub, AProductionsite* _siteRef)
{
	if (subscribedWorker_UnasignedPool.Contains(_toUnsub))
		subscribedWorker_UnasignedPool.Remove(_toUnsub);
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, !subscribedWorker.Contains(_toUnsub)"))

	_toUnsub->SetWorkerState(EWorkerStatus::WS_Assigned_MainJob, _siteRef);
	WorkerWorldManagerUpdate(_toUnsub);
}

void UProductionSiteManager::UnsubscribeWorkerToWorldPool(AWorker* _toUnsub, EPlayerIdent _playerIdent)
{
	if (subscribedWorker_HiredPool.Contains(_toUnsub))
	{
		subscribedWorker_HiredPool.Remove(_toUnsub);
		subscribedWorker_UnasignedPool.Remove(_toUnsub);

		_toUnsub->SetWorkerState(EWorkerStatus::WS_Unemployed);
		WorkerWorldManagerUpdate(_toUnsub);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, !subscribedWorker.Contains(_toUnsub)"))
}

void UProductionSiteManager::WorkerWorldManagerUpdate(AWorker* _toUpdate)
{
	workerWorldManager->UpdateWorkerStatus(_toUpdate, managerOwner->GetPlayerIdent());
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
