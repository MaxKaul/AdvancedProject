// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder.h"

#include "BuildingSite.h"
#include "Productionsite.h"
#include "PlayerBase.h"
#include "ProductionSiteManager.h"
#include "WorkerWorldManager.h"

UBuilder::UBuilder()
{
	PrimaryComponentTick.bCanEverTick = true;

	ZOffset = 300.f;
}


void UBuilder::BeginPlay()
{
	Super::BeginPlay();
}


void UBuilder::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UBuilder::InitBuilder(UProductionSiteManager* _manager, AMarketManager* _marketManager, APlayerBase* _builderOwner, AWorkerWorldManager* _workerWorldManager)
{
	bool status = false;

	world = GetWorld();
	marketManager = _marketManager;
	productionSiteManager = _manager;
	builderOwner = _builderOwner;
	workerWorldManager = _workerWorldManager;

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
bool UBuilder::BuildProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite, class AMarketManager* _marketManager, int _siteID)
{
	bool status = false;

	if(!_buildingSite)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBuilder, !_buildingSite"));
		return false;
	}

	// Jeder buildingsdite hat erlaubte productiontype, hier teste ich gegen
	for(EProductionSiteType type : _buildingSite->GetAllowedTypes())
	{
		if (_type == type)
		{
			if (_buildingSite->GetHasBeenBuildOn())
				break;

			FVector spawnpos = _buildingSite->GetActorLocation();
			FRotator spawnrot = _buildingSite->GetActorRotation();

			// Debug weil die geb�ude meshes noch angepasst werden m�ssen
			spawnpos.Z -= ZOffset;

			AProductionsite* spawnedsite = Cast<AProductionsite>(world->SpawnActor(productionSiteClass, &spawnpos, &spawnrot));

			FPS_OverloadFuncs* ps_overloadfuncs;
			ps_overloadfuncs = new FPS_OverloadFuncs(spawnedsite);

			ps_overloadfuncs->InitProductionSite(_siteMesh,_type,_buildingSite, marketManager, _siteID, builderOwner, ps_overloadfuncs, productionSiteManager);

			productionSiteManager->SubscribeProductionsite(spawnedsite);
			_buildingSite->SetBuildStatus(true);
			workerWorldManager->AddProductionSite(spawnedsite);

			status = true;
			break;
		}
	}

	if(!status && !_buildingSite->GetHasBeenBuildOn())
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
