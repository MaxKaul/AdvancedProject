// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder.h"

#include "BuildingSite.h"
#include "Productionsite.h"
#include "ProductionSiteManager.h"

UBuilder::UBuilder()
{

	PrimaryComponentTick.bCanEverTick = true;
}


void UBuilder::BeginPlay()
{
	Super::BeginPlay();
}


void UBuilder::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UBuilder::InitBuilder(AProductionSiteManager* _manager, AMarketManager* _marketManager)
{
	bool status = false;

	world = GetWorld();
	marketManager = _marketManager;
	productionSiteManager = _manager;

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager, !NullcheckDependencies"));
		return false;
	}
	else
		status = true;

	productionSiteManager = _manager;

	return status;
}

// Eine ProductionSite wird mit einem savedata struct initialised (Einfach damit ich das nicht doppelt machen muss, die save data soll die selben daten speichern wie die welche ben�tigt werden zum inin)
// Dran denken; Ohne savedata k�nnen zum start keine BuildProductionSite existieren
bool UBuilder::BuildProductionSite(FProductionSiteSaveData _siteData)
{
	bool status = false;

	// Jeder buildingsdite hat erlaubte productiontype, hier teste ich gegen
	for(EProductionSiteType type : _siteData.GetSavedBuildingSite()->GetAllowedTypes())
	{
		if (_siteData.GetSavedType() == type)
		{
			FVector spawnpos = _siteData.GetSavedBuildingSite()->GetActorLocation();
			FRotator spawnrot = _siteData.GetSavedBuildingSite()->GetActorRotation();

			// Debug weil die geb�ude meshes noch angepasst werden m�ssen
			spawnpos.Z -= 300.f;

			AProductionsite* spawnedsite = Cast<AProductionsite>(world->SpawnActor(productionSiteClass, &spawnpos, &spawnrot));
			spawnedsite->InitProductionSite(_siteData.GetSavedMesh(), _siteData.GetSavedType(), _siteData.GetSavedBuildingSite(), marketManager);

			productionSiteManager->SubscribeProductionsite(spawnedsite);
			_siteData.GetSavedBuildingSite()->SetBuildStatus(true);

			status = true;
			break;
		}
	}


	if(!status)
		UE_LOG(LogTemp, Warning, TEXT("Wrong building type"));

	return status;
}


bool UBuilder::NullcheckDependencies()
{
	bool status = true;

	if(!productionSiteManager)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("UBuilder !productionSiteManager"));
	}

	if (!world)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("UBuilder !world"));
	}

	if (!marketManager)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("UBuilder !marketManager"));
	}

	if (!productionSiteClass)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("UBuilder !productionSiteClass"));
	}

	return status;
}