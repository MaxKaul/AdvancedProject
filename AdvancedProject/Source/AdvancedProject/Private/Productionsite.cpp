// Fill out your copyright notice in the Description page of Project Settings.

#include "Productionsite.h"
#include "BuildingSite.h"

AProductionsite::AProductionsite()
{
	PrimaryActorTick.bCanEverTick = true;

	actorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Actor Mesh");
	RootComponent = actorMeshComponent;
}

void AProductionsite::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProductionsite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProductionsite::InitProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite)
{
	if(!NullcheckDependencies())
	{
		UE_LOG(LogTemp,Warning,TEXT("AProductionsite, !NullcheckDependencies()"))
		return;
	}

	productionSiteType = _type;
	actorMeshComponent->SetStaticMesh(_siteMesh);
}

FProductionSiteSaveData AProductionsite::GetProductionSiteSaveData()
{
	FProductionSiteSaveData savedata =
	{
		actorMesh,
		productionSiteType,
		buildingSite
	};

	return savedata;
}


bool AProductionsite::NullcheckDependencies()
{
	bool status = false;

	if (actorMeshComponent)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite !actorMeshComponent"));

	if (actorMesh)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite !actorMesh"));

	if (buildingSite)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite !buildingSite"));

	if (productionSiteType == EProductionSiteType::PST_DEFAULT)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite !EProductionSiteType::PST_DEFAULT"));

	return status;
}
