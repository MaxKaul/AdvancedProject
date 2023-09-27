// Fill out your copyright notice in the Description page of Project Settings.


#include "ProductionSiteManager.h"

#include "BuildingSite.h"
#include "Builder.h"
#include "Productionsite.h"

// Sets default values
AProductionSiteManager::AProductionSiteManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//TESTBUILDERCOMP = CreateDefaultSubobject<UBuilder>("SS");
	TESTBUILDERCOMP = CreateDefaultSubobject<UBuilder>("dd");

}

// Called when the game starts or when spawned
void AProductionSiteManager::BeginPlay()
{
	Super::BeginPlay();


	/*Zu testzwecken als solches implementiert, hat von spieler/AI aus gespawned zu werden, von diesem aus wird auch die FProductionSiteManagerSaveData übertragen
	 *TArray<FProductionSiteSaveData> testproductionsitedata =
	{
		FProductionSiteSaveData {TESTMESHES[0], EProductionSiteType::PST_Fruits, TESTSITES[0] },
		FProductionSiteSaveData {TESTMESHES[1], EProductionSiteType::PST_Furniture_Jewelry, TESTSITES[1] },
		FProductionSiteSaveData {TESTMESHES[2], EProductionSiteType::PST_Ambrosia, TESTSITES[2] }
	};

	FProductionSiteManagerSaveData testsavedata=
	{
		testproductionsitedata
	};

	InitProductionSiteManager(nullptr, testsavedata);
	 */

	TESTBUILDERCOMP->InitBuilder(this);
	TESTBUILDERCOMP->BuildProductionSite(EProductionSiteType::PST_Ambrosia, TESTSITES[0], TESTMESHES[0]);
}

// Called every frame
void AProductionSiteManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AProductionSiteManager::InitProductionSiteManager(AActor* _managerOwner)
{
	world = GetWorld();
	managerOwner = _managerOwner;

	if(!NullcheckDependencies())
	{
		UE_LOG(LogTemp,Warning,TEXT("AProductionSiteManager, !NullcheckDependencies()"))
		return;
	}
}


void AProductionSiteManager::InitProductionSiteManager(AActor* _managerOwner, FProductionSiteManagerSaveData _saveData)
{
	world = GetWorld();
	managerOwner = _managerOwner;

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AProductionSiteManager, !NullcheckDependencies()"))
			return;
	}

	InitAllProductionSites(_saveData);
}

void AProductionSiteManager::InitAllProductionSites(FProductionSiteManagerSaveData _saveData)
{
	// -> Hier würde ich die save data entgegen nehmen

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

		spawnedsite->InitProductionSite(savedmesh ,savedtype, savedbuildingsite);

		allProductionSites.Add(spawnedsite);
		
		savedbuildingsite->SetBuildStatus(true);
	}
}

void AProductionSiteManager::SubscribeProductionsite(AProductionsite* _newProductionSite)
{
	if(allProductionSites.Contains(_newProductionSite))
	{
		UE_LOG(LogTemp,Warning,TEXT("AProductionSiteManager, allProductionSites.Contains(_newProductionSite)"))
		return;
	}

	allProductionSites.Add(_newProductionSite);
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
	bool status = false;

	if (productionSiteClass)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager !marketStall"));

	if (world)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager !world"));

	if (managerOwner)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager !world"));

	return status;
}
