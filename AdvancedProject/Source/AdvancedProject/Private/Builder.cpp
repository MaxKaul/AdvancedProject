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

// Eine ProductionSite wird mit einem savedata struct initialised (Einfach damit ich das nicht doppelt machen muss, die save data soll die selben daten speichern wie die welche benötigt werden zum inin)
bool UBuilder::BuildProductionSite(FProductionSiteSaveData _siteData)
{
	bool status = false;

	for(EProductionSiteType type : _siteData.GetSavedBuildingSite()->GetAllowedTypes())
	{
		if (_siteData.GetSavedType() == type)
		{
			FVector spawnpos = _siteData.GetSavedBuildingSite()->GetActorLocation();
			FRotator spawnrot = _siteData.GetSavedBuildingSite()->GetActorRotation();

			// Debug weil die gebäude meshes noch angepasst werden müssen
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
	bool status = false;

	if (!productionSiteManager)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("UBuilder !productionSiteManager"));
	
	if (!world)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("UBuilder !world"));

	if (!marketManager)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("UBuilder !marketManager"));

	//if(!productionSiteClass)
	//	status = true;
	//else
	//	UE_LOG(LogTemp, Warning, TEXT("UBuilder !productionSiteClass"));

	return status;
}