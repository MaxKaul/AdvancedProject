// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder.h"

#include "BuildingSite.h"
#include "Productionsite.h"
#include "PlayerBase.h"
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

bool UBuilder::InitBuilder(UProductionSiteManager* _manager, AMarketManager* _marketManager, APlayerBase* _builderOwner)
{
	bool status = false;

	world = GetWorld();
	marketManager = _marketManager;
	productionSiteManager = _manager;
	builderOwner = _builderOwner;

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
// Dran denken; Ohne savedata können zum start keine BuildProductionSite existieren
bool UBuilder::BuildProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite, class AMarketManager* _marketManager, int _siteID)
{
	bool status = false;

	// Jeder buildingsdite hat erlaubte productiontype, hier teste ich gegen
	for(EProductionSiteType type : _buildingSite->GetAllowedTypes())
	{
		if (_type == type)
		{
			if (_buildingSite->GetBuildStatus())
				break;

			FVector spawnpos = _buildingSite->GetActorLocation();
			FRotator spawnrot = _buildingSite->GetActorRotation();

			// Debug weil die gebäude meshes noch angepasst werden müssen
			spawnpos.Z -= 300.f;

			AProductionsite* spawnedsite = Cast<AProductionsite>(world->SpawnActor(productionSiteClass, &spawnpos, &spawnrot));

			spawnedsite->InitProductionSite(_siteMesh,_type,_buildingSite, marketManager, _siteID, builderOwner);

			productionSiteManager->SubscribeProductionsite(spawnedsite);
			_buildingSite->SetBuildStatus(true);

			status = true;
			break;
		}
	}

	if(!status && !_buildingSite->GetBuildStatus())
	{
		UE_LOG(LogTemp, Warning, TEXT("UBuilder, Wrong building type"));
	}
	else if(!status)
		UE_LOG(LogTemp, Warning, TEXT("UBuilder, Site is occupied"));

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
