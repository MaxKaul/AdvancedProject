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
	resourceStdTickValue = 1.f;
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

// In beide ticks m�ssen die arbeietr mit einfaktoriert werden
// Resourcen sollen immer nuck ticken solange ihr arbeiter zugewiesen wurden

// Wir looen durch alle resourcespairs und checken on die callende resource in der lokalen map ist
// Dies mach ich damit ich eine refferenz auf die callende resource habe, welche ich dann �ber deren TickResource auf den neuen wert erh�he
// Danach resette ich den timer der resource
void AProductionsite::TickResourceProduction(EResourceIdent _resourceIdent)
{
	for (TTuple<FProductionResources, FTimerHandle> resourcehandlepair : productionResourceHandlePair)
	{
		if (_resourceIdent == resourcehandlepair.Key.GetResourceIdent())
		{
			//resourcehandlepair.Key.TickResource(resourcehandlepair.Key.GetResourceTickRate());
	
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
			bool bcanafford = false;
			int index = 0;

			for (TTuple<EResourceIdent, int> item : _resourceCost)
			{
				index++;

				if (productionSiteResourcePool.FindRef(item.Key) >= item.Value)
					if (index >= _resourceCost.Num())
						bcanafford = true;
			}

			if(!bcanafford)
			{
				UE_LOG(LogTemp, Warning, TEXT("AProductionsite, Not enough resources to tick resource with cost"));
				return;
			}

			for (TTuple<EResourceIdent, int> item : _resourceCost)
			{
				productionSiteResourcePool.Add(item.Key, productionSiteResourcePool.FindRef(item.Key) - item.Value);
			}

			productionSiteResourcePool.Add(EResourceIdent::ERI_Furniture, productionSiteResourcePool.FindRef(EResourceIdent::ERI_Furniture) + resourceStdTickValue);


			FTimerDelegate  currdelegate;
			currdelegate.BindUFunction(this, FName("TickResourceProduction"), resourcehandlepair.Key.GetResourceIdent(), resourcehandlepair.Key.GetResourceCost());

			world->GetTimerManager().SetTimer(resourcehandlepair.Value, currdelegate, resourcehandlepair.Key.GetResourceTickRate(), false);

			break;
		}
	}
}

// Ich checke welcher resourcentyp mit welcher production site �bereinstimmt und adde dann das pair
// Dann werden f�r alle pairs die resourcetick gecalled, in ihr wird nach dem ersten tick der timer f�r jede resoruce gestellt
void AProductionsite::InitResources()
{
	// Resource in pool adden

	TMap<EResourceIdent, FIndividualResourceInfo> poolinfo = marketManager->GetPoolInfo();

	for(TTuple<EResourceIdent, FIndividualResourceInfo> item: poolinfo)
	{
		if (productionSiteType != item.Value.GetAllowedProductionSites())
			return;

		//FProductionResources currResource =
		//{
		//	item.Key,
		//	item.Value.GetResourceAmount(),
		//	item.Value.GetResourceTickRate(),
		//	FString::FromInt((int)item.Value.GetResourceIdent()),
		//	FString::FromInt((int)item.Value.GetAllowedProductionSites()),
		//	item.Value.GetHasCost(),
		//	item.Value.GetResourceCost()
		//};

		//productionResourceHandlePair.Add(currResource, FTimerHandle());
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
