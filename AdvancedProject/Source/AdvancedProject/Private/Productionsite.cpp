// Fill out your copyright notice in the Description page of Project Settings.

#include "Productionsite.h"
#include "BuildingSite.h"
#include "MarketManager.h"
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
}

void AProductionsite::InitProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite,  AMarketManager* _marketManager)
{
	world = GetWorld();
	productionSiteType = _type;
	actorMeshComponent->SetStaticMesh(_siteMesh);
	marketManager = _marketManager;

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
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, !actorMeshComponent"));
	}

	if (!actorMesh)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, !actorMesh"));
	}

	if (!buildingSite)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, !buildingSite"));
	}

	if (productionSiteType == EProductionSiteType::PST_DEFAULT)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, !EProductionSiteType::PST_DEFAULT"));
	}

	if (!world)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, !world"));
	}

	return status;
}

TArray<FProductionResources> AProductionsite::GetCurrentResources()
{
	TArray<FProductionResources> productionresources;

	//productionResourceHandlePair.GenerateKeyArray(productionresources);

	return productionresources;
}

void AProductionsite::TickResourceProduction(EResourceIdent _resourceIdent)
{
	//for (TTuple<FProduktionResources, FTimerHandle> resourcehandlepair : productionResourceHandlePair)
	//{
	//	if (_resourceIdent == resourcehandlepair.Key.GetResourceIdent())
	//	{
	//		resourcehandlepair.Key.TickResource(resourceTickValue);
	//
	//		FTimerDelegate  currdelegate;
	//		currdelegate.BindUFunction(this, FName("TickResourceProduction"), resourcehandlepair.Key.GetResourceIdent());
	//
	//		world->GetTimerManager().SetTimer(resourcehandlepair.Value, currdelegate, resourcehandlepair.Key.GetResourceTickRate(), false);
	//	}
	//}
}

void AProductionsite::InitResources()
{
	//TMap<EResourceIdent, FIndividualResourceInfo> poolinfo = marketManager->GetPoolInfo();
	//
	//for(TTuple<EResourceIdent, FIndividualResourceInfo> item: poolinfo)
	//{
	//	if(productionSiteType == item.Value.GetAllowedProductionSites())
	//	{
	//		switch (productionSiteType)
	//		{
	//		case EProductionSiteType::PST_Meat:
	//			productionResourceHandlePair.Add({  FProduktionResources(EResourceIdent::ERI_Meat, 0, item.Value.GetResourceTickRate()), FTimerHandle() });
	//			break;
	//		case EProductionSiteType::PST_Fruits:
	//			productionResourceHandlePair.Add({  FProduktionResources(EResourceIdent::ERI_Fruit, 0, item.Value.GetResourceTickRate()), FTimerHandle() });
	//			break;
	//		case EProductionSiteType::PST_Hardwood_Resin:
	//			productionResourceHandlePair.Add({  FProduktionResources(EResourceIdent::ERI_Hardwood, 0, item.Value.GetResourceTickRate()), FTimerHandle() });
	//			productionResourceHandlePair.Add({  FProduktionResources(EResourceIdent::ERI_Resin, 0, item.Value.GetResourceTickRate()), FTimerHandle() });
	//			break;
	//		case EProductionSiteType::PST_Furniture_Jewelry:
	//			productionResourceHandlePair.Add({  FProduktionResources(EResourceIdent::ERI_Furniture, 0, item.Value.GetResourceTickRate()), FTimerHandle() });
	//			productionResourceHandlePair.Add({  FProduktionResources(EResourceIdent::ERI_Jewelry, 0, item.Value.GetResourceTickRate()), FTimerHandle() });
	//			break;
	//		case EProductionSiteType::PST_Ambrosia:
	//			productionResourceHandlePair.Add({  FProduktionResources(EResourceIdent::ERI_Ambrosia, 0, item.Value.GetResourceTickRate()), FTimerHandle() });;
	//			break;
	//		case EProductionSiteType::PST_Wheat:
	//			productionResourceHandlePair.Add({  FProduktionResources(EResourceIdent::ERI_Wheat, 0, item.Value.GetResourceTickRate()), FTimerHandle() });
	//			break;
	//
	//		default:
	//			UE_LOG(LogTemp, Warning, TEXT("AProductionsite, resourcesAmountPairs could not Init"))
	//				break;
	//		}
	//	}
	//}
	//
	//
	//if (productionResourceHandlePair.Num() > 0)
	//{
	//	for (TTuple<FProduktionResources, FTimerHandle> resourcehandlepair : productionResourceHandlePair)
	//		TickResourceProduction(resourcehandlepair.Key.GetResourceIdent());
	//}
	//else
	//	UE_LOG(LogTemp,Warning,TEXT("AProductionsite, productionResource.Num() < 0"))
}
