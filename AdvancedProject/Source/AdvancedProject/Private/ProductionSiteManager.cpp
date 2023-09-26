// Fill out your copyright notice in the Description page of Project Settings.


#include "ProductionSiteManager.h"

#include "BuildingSite.h"
#include "Productionsite.h"

// Sets default values
AProductionSiteManager::AProductionSiteManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProductionSiteManager::BeginPlay()
{
	Super::BeginPlay();

	InitProductionSiteManager();
}

// Called every frame
void AProductionSiteManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AProductionSiteManager::InitProductionSiteManager()
{
	world = GetWorld();

	if(!NullcheckDependencies())
	{
		UE_LOG(LogTemp,Warning,TEXT("AProductionSiteManager, !NullcheckDependencies()"))
		return;
	}

	InitAllProductionSites();
}

void AProductionSiteManager::InitAllProductionSites()
{
	// -> Hier würde ich die save data entgegen nehmen

	EProductionSiteType type = EProductionSiteType::PST_Ambrosia;

	if (buildingSites.Num() <= 0)
		return;

	for (ABuildingSite* site : buildingSites)
	{
		FVector testspawnpos = site->GetActorLocation();
		FRotator testspawnrot = site->GetActorRotation();

		testspawnpos.Z -= 300.f;

		AProductionsite* testzsite = Cast<AProductionsite>(world->SpawnActor(productionSiteClass, &testspawnpos, &testspawnrot));

		testzsite->InitProductionSite(siteKeyMeshPair.FindRef(type), type);

		allProductionSites.Add(testzsite);
		site->SetBuildStatus(true);
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

	return status;
}