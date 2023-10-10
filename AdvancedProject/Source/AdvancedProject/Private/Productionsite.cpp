// Fill out your copyright notice in the Description page of Project Settings.

#include "Productionsite.h"

#include <string>

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

void AProductionsite::InitProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite,  AMarketManager* _marketManager, int _siteID,
										 TMap<EResourceIdent, int> _productionSiteResourcePool)
{
	world = GetWorld();
	productionSiteType = _type;
	actorMesh = _siteMesh;
	actorMeshComponent->SetStaticMesh(_siteMesh);
	marketManager = _marketManager;
	buildingSite = _buildingSite;
	siteID = _siteID;
	productionSiteResourcePool = _productionSiteResourcePool;

	InitResources();
}

FProductionSiteSaveData AProductionsite::GetProductionSiteSaveData()
{
	FProductionSiteSaveData savedata =
	{
		actorMesh,
		productionSiteType,
		buildingSite,
		FString::FromInt(siteID) + FString::FromInt((int)productionSiteType),
		FString::FromInt(siteID),
		productionSiteResourcePool
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

	productionResourceHandlePair.GenerateKeyArray(productionresources);

	return productionresources;
}

// In beide ticks müssen die arbeietr mit einfaktoriert werden
// Resourcen sollen immer nuck ticken solange ihr arbeiter zugewiesen wurden

// Wir looen durch alle resourcespairs und checken on die callende resource in der lokalen map ist
// Dies mach ich damit ich eine refferenz auf die callende resource habe, welche ich dann über deren TickResource auf den neuen wert erhöhe
// Danach resette ich den timer der resource
void AProductionsite::TickResourceProduction(EResourceIdent _resourceIdent)
{
	for (TTuple<FProductionResources, FTimerHandle> resourcehandlepair : productionResourceHandlePair)
	{
		if (_resourceIdent == resourcehandlepair.Key.GetResourceIdent())
		{
			resourcehandlepair.Key.TickResource(resourcehandlepair.Key.GetResourceTickRate());
	
			FTimerDelegate  currdelegate;
			currdelegate.BindUFunction(this, FName("TickResourceProduction"), resourcehandlepair.Key.GetResourceIdent());
	
			world->GetTimerManager().SetTimer(resourcehandlepair.Value, currdelegate, resourcehandlepair.Key.GetResourceTickRate(), false);

			break;
		}
	}
}

// _resourceIdent ist produziert wird, _resourceCost representiert die kosten einer einheit
void AProductionsite::TickResourceProduction(EResourceIdent _resourceIdent, TMap<EResourceIdent, int> _resourceCost)
{
	for (TTuple<FProductionResources, FTimerHandle> resourcehandlepair : productionResourceHandlePair)
	{
		if (_resourceIdent == resourcehandlepair.Key.GetResourceIdent())
		{
			for (TTuple<EResourceIdent, int> item : _resourceCost)
			{
				if(productionSiteResourcePool.FindRef(item.Key) >= item.Value)
				{
					UE_LOG(LogTemp, Warning, TEXT("DDDDDDDDDDDDDDD"))

					resourcehandlepair.Key.TickResource(resourcehandlepair.Key.GetResourceTickRate());

					int newamount = productionSiteResourcePool.FindRef(item.Key) - item.Value;
					productionSiteResourcePool.Add(item.Key, newamount);
				}
				else
					UE_LOG(LogTemp,Warning,TEXT("AProductionsite, Not enough resources to tick resource with cost"))
			}

			FTimerDelegate  currdelegate;
			currdelegate.BindUFunction(this, FName("TickResourceProduction"), resourcehandlepair.Key.GetResourceIdent(), resourcehandlepair.Key.GetResourceCost());

			world->GetTimerManager().SetTimer(resourcehandlepair.Value, currdelegate, resourcehandlepair.Key.GetResourceTickRate(), false);

			break;
		}
	}
}

// Ich checke welcher resourcentyp mit welcher production site übereinstimmt und adde dann das pair
// Dann werden für alle pairs die resourcetick gecalled, in ihr wird nach dem ersten tick der timer für jede resoruce gestellt
void AProductionsite::InitResources()
{
	// Resource in pool adden

	TMap<EResourceIdent, FIndividualResourceInfo> poolinfo = marketManager->GetPoolInfo();

	for(TTuple<EResourceIdent, FIndividualResourceInfo> item: poolinfo)
	{
		if(productionSiteType == item.Value.GetAllowedProductionSites())
		{
			switch (productionSiteType)
			{
			case EProductionSiteType::PST_Meat:
				productionResourceHandlePair.Add({ FProductionResources(EResourceIdent::ERI_Meat, 0, item.Value.GetResourceTickRate(),"meat", FString::FromInt((int)EResourceIdent::ERI_Meat), 
					item.Value.GetHasCost(), item.Value.GetResourceCost()), FTimerHandle()});
				break;
			case EProductionSiteType::PST_Fruits:
				productionResourceHandlePair.Add({ FProductionResources(EResourceIdent::ERI_Fruit, 0, item.Value.GetResourceTickRate(),"fruit",FString::FromInt((int)EResourceIdent::ERI_Fruit), 
					item.Value.GetHasCost(), item.Value.GetResourceCost()), FTimerHandle() });
				break;
			case EProductionSiteType::PST_Hardwood_Resin:
				productionResourceHandlePair.Add({ FProductionResources(EResourceIdent::ERI_Hardwood, 0, item.Value.GetResourceTickRate(),"hardwood",FString::FromInt((int)EResourceIdent::ERI_Hardwood), 
					item.Value.GetHasCost(), item.Value.GetResourceCost()), FTimerHandle() });
				productionResourceHandlePair.Add({ FProductionResources(EResourceIdent::ERI_Resin, 0, item.Value.GetResourceTickRate(),"resin",FString::FromInt((int)EResourceIdent::ERI_Resin), 
					item.Value.GetHasCost(), item.Value.GetResourceCost()), FTimerHandle() });
				break;
			case EProductionSiteType::PST_Furniture_Jewelry:
				productionResourceHandlePair.Add({ FProductionResources(EResourceIdent::ERI_Furniture, 0, item.Value.GetResourceTickRate(),"furniture",FString::FromInt((int)EResourceIdent::ERI_Furniture), 
					item.Value.GetHasCost(), item.Value.GetResourceCost()), FTimerHandle() });
				productionResourceHandlePair.Add({ FProductionResources(EResourceIdent::ERI_Jewelry, 0, item.Value.GetResourceTickRate(),"jewelry",FString::FromInt((int)EResourceIdent::ERI_Jewelry), 
					item.Value.GetHasCost(), item.Value.GetResourceCost()), FTimerHandle() });
				break;
			case EProductionSiteType::PST_Ambrosia:
				productionResourceHandlePair.Add({ FProductionResources(EResourceIdent::ERI_Ambrosia, 0, item.Value.GetResourceTickRate(),"ambrosia",FString::FromInt((int)EResourceIdent::ERI_Ambrosia), 
					item.Value.GetHasCost(), item.Value.GetResourceCost()), FTimerHandle() });;
				break;
			case EProductionSiteType::PST_Wheat:
				productionResourceHandlePair.Add({ FProductionResources(EResourceIdent::ERI_Wheat, 0, item.Value.GetResourceTickRate(),"wheat",FString::FromInt((int)EResourceIdent::ERI_Wheat), 
					item.Value.GetHasCost(), item.Value.GetResourceCost()), FTimerHandle() });
				break;
		
			default:
				UE_LOG(LogTemp, Warning, TEXT("AProductionsite, resourcesAmountPairs could not Init"))
					break;
			}
		}
	}
	
	if (productionResourceHandlePair.Num() > 0)
	{
		for (TTuple<FProductionResources, FTimerHandle> resourcehandlepair : productionResourceHandlePair)
		{
			if (resourcehandlepair.Key.GetHasCost())
				TickResourceProduction(resourcehandlepair.Key.GetResourceIdent(), resourcehandlepair.Key.GetResourceCost());
			else
				TickResourceProduction(resourcehandlepair.Key.GetResourceIdent());
		}
	}
	else
		UE_LOG(LogTemp,Warning,TEXT("AProductionsite, productionResource.Num() < 0"))
}
