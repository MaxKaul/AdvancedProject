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

bool UBuilder::InitBuilder(AProductionSiteManager* _manager)
{
	bool status = false;

	world = GetWorld();

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

bool UBuilder::BuildProductionSite(EProductionSiteType _productionSiteType, ABuildingSite* _buildingSite, UStaticMesh* _buildingMesh)
{
	bool status = false;

	for(EProductionSiteType type : _buildingSite->GetAllowedTypes())
	{
		if (_productionSiteType == type)
		{
			FVector spawnpos = _buildingSite->GetActorLocation();
			FRotator spawnrot = _buildingSite->GetActorRotation();

			// Debug weil die gebäude meshes noch angepasst werden müssen
			spawnpos.Z -= 300.f;

			AProductionsite* spawnedsite = Cast<AProductionsite>(world->SpawnActor(productionSiteClass, &spawnpos, &spawnrot));

			spawnedsite->InitProductionSite(_buildingMesh, _productionSiteType, _buildingSite);


			productionSiteManager->SubscribeProductionsite(spawnedsite);
			_buildingSite->SetBuildStatus(true);

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

	//if(!productionSiteClass)
	//	status = true;
	//else
	//	UE_LOG(LogTemp, Warning, TEXT("UBuilder !productionSiteClass"));

	return status;
}