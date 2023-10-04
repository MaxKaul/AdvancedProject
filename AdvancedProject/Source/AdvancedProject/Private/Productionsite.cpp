// Fill out your copyright notice in the Description page of Project Settings.

#include "Productionsite.h"
#include "BuildingSite.h"
#include "ResourceTableBase.h"

AProductionsite::AProductionsite()
{
	PrimaryActorTick.bCanEverTick = true;

	actorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Actor Mesh");
	RootComponent = actorMeshComponent;
}

void AProductionsite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bWasInit)
		return;

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, !NullcheckDependencies()"))
		return;
	}

	TickResourceProduction();
}

void AProductionsite::InitProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite)
{
	productionSiteType = _type;
	actorMeshComponent->SetStaticMesh(_siteMesh);

	InitResources();
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
	bool status = true;

	if (!actorMeshComponent)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite !actorMeshComponent"));
	}

	if (!actorMesh)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite !actorMesh"));
	}

	if (!buildingSite)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite !buildingSite"));
	}

	if (productionSiteType == EProductionSiteType::PST_DEFAULT)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite !EProductionSiteType::PST_DEFAULT"));
	}

	return status;
}

void AProductionsite::TickResourceProduction()
{

}

void AProductionsite::InitResources()
{

	TArray<FResourceTableBase*> resourcebasevalues;

	//for (TTuple<FName, unsigned char*> rowitem : resourceDefaultTable->GetRowMap())
	//{
	//	resourcebasevalues.Add(reinterpret_cast<FResourceTableBase*>(rowitem.Value));
	//}

	//for (size_t i = 0; i < resourcebasevalues.Num(); i++)
	//{
	//	if(resourcebasevalues[i]->Resource)
	//}

	switch (productionSiteType)
	{
	case EProductionSiteType::PST_Meat:
		productionResource.Add({ EResourceIdent::ERI_Meat, 0, 5.f});
		break;
	case EProductionSiteType::PST_Fruits:
		productionResource.Add({ EResourceIdent::ERI_Fruit, 0, 5.f });
		break;
	case EProductionSiteType::PST_Hardwood_Resin:
		productionResource.Add({ EResourceIdent::ERI_Hardwood, 0, 5.f });
		productionResource.Add({ EResourceIdent::ERI_Resin, 0, 5.f });
		break;
	case EProductionSiteType::PST_Furniture_Jewelry:
		productionResource.Add({ EResourceIdent::ERI_Furniture, 0, 5.f });
		productionResource.Add({ EResourceIdent::ERI_Jewelry, 0, 5.f });
		break;
	case EProductionSiteType::PST_Ambrosia:
		productionResource.Add({ EResourceIdent::ERI_Ambrosia, 0, 5.f });
		break;
	case EProductionSiteType::PST_Wheat:
		productionResource.Add({ EResourceIdent::ERI_Wheat, 0, 5.f });
		break;

	default:
	UE_LOG(LogTemp,Warning,TEXT("AProductionsite, resourcesAmountPairs could not Init"))
		break;
	}
}
