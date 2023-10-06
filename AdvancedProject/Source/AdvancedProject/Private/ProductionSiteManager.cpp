// Fill out your copyright notice in the Description page of Project Settings.


#include "ProductionSiteManager.h"

#include "Productionsite.h"
#include "BuildingSite.h"
#include "Builder.h"

// Sets default values
AProductionSiteManager::AProductionSiteManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TESTBUILDERCOMP = CreateDefaultSubobject<UBuilder>("DD");
}

// Called when the game starts or when spawned
void AProductionSiteManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AProductionSiteManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AProductionSiteManager, !NullcheckDependencies()"))
		return;
	}
}


void AProductionSiteManager::InitProductionSiteManager(AActor* _managerOwner, AMarketManager* _marketManager, ABuildingSite* _TESTBSITE)
{
	world = GetWorld();
	managerOwner = _managerOwner;
	marketManager = _marketManager;

	if(!NullcheckDependencies())
	{
		UE_LOG(LogTemp,Warning,TEXT("AProductionSiteManager, !NullcheckDependencies()"))
		return;
	}

	TESTBUILDERCOMP->InitBuilder(this, marketManager);
	FProductionSiteSaveData testsavedata = { TESTMESHES[0],EProductionSiteType::PST_Furniture_Jewelry,_TESTBSITE };
	TESTBUILDERCOMP->BuildProductionSite(testsavedata);
}


void AProductionSiteManager::InitProductionSiteManager(AActor* _managerOwner, FProductionSiteManagerSaveData _saveData, AMarketManager* _marketManager)
{
	world = GetWorld();
	managerOwner = _managerOwner;
	marketManager = _marketManager;

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AProductionSiteManager, !NullcheckDependencies()"))
			return;
	}

	// Raus um zu testen
	//InitAllProductionSites(_saveData);
}

void AProductionSiteManager::InitAllProductionSites(FProductionSiteManagerSaveData _saveData)
{
	// -> Hier nehme ich die save data entgegen nehmen

	TArray<FProductionSiteSaveData> allsavedsites = _saveData.GetSaveData();

	if (allsavedsites.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AProductionSiteManager, Init from save data was called, but no save data was found"))
		return;
	}


	for (FProductionSiteSaveData sitedata : allsavedsites)
	{
		UStaticMesh* savedmesh = sitedata.GetSavedMesh();
		EProductionSiteType savedtype = sitedata.GetSavedType();
		ABuildingSite* savedbuildingsite = sitedata.GetSavedBuildingSite();

		FVector spawnpos = savedbuildingsite->GetActorLocation();
		FRotator spawnrot = savedbuildingsite->GetActorRotation();

		// Debug weil die gebäude meshes noch angepasst werden müssen
		spawnpos.Z -= 300.f;

		AProductionsite* spawnedsite = Cast<AProductionsite>(world->SpawnActor(productionSiteClass, &spawnpos, &spawnrot));

		spawnedsite->InitProductionSite(savedmesh ,savedtype, savedbuildingsite, marketManager);

		allProductionSites.Add(spawnedsite);
		
		savedbuildingsite->SetBuildStatus(true);
	}
}

void AProductionSiteManager::SubscribeProductionsite(AProductionsite* _newProductionSite)
{
	//if(!allProductionSites.IsEmpty() && allProductionSites.Contains(_newProductionSite))
	//{
	//	UE_LOG(LogTemp,Warning,TEXT("AProductionSiteManager, allProductionSites.Contains(_newProductionSite)"))
	//	return;
	//}
	//
	//allProductionSites.Add(_newProductionSite);
}

FProductionSiteManagerSaveData AProductionSiteManager::GetProductionSiteManagerSaveData()
{
	TArray<FProductionSiteSaveData> allpssavedata;
	
	for(AProductionsite* site : allProductionSites)
	{
		FProductionSiteSaveData savedata = site->GetProductionSiteSaveData();

		allpssavedata.Add(savedata);

		/* 	Geht net weril kein linksseiter == operand für das struct geht (nehm ich an)
			if (!allpssavedata.Contains(savedata))
				allpssavedata.Add(savedata);
			else
				UE_LOG(LogTemp, Warning, TEXT("AProductionSiteManager, Production Site save data duplicate"))
		 */
	}
	
	FProductionSiteManagerSaveData savedata =
	{
		allpssavedata
	};

	return savedata;
}


bool AProductionSiteManager::NullcheckDependencies()
{
	bool status = true;

	if(!productionSiteClass)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionSiteManager !marketStall"));
	}

	if (!world)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionSiteManager !world"));
	}

	if (!managerOwner)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionSiteManager !managerOwner"));
	}

	if (!marketManager)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionSiteManager !marketManager"));
	}

	return status;
}
