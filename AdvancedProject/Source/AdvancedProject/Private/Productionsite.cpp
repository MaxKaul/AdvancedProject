// Fill out your copyright notice in the Description page of Project Settings.

#include "Productionsite.h"

#include <string>

#include "BuildingSite.h"
#include "MarketManager.h"
#include "PlayerBase.h"
#include "ResourceTableBase.h"

#include "Worker.h"

AProductionsite::AProductionsite()
{
	PrimaryActorTick.bCanEverTick = true;

	actorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Actor Mesh");
	RootComponent = actorMeshComponent;

	resourceStdTickValue = 1.f;
	productionSiteProductivity = 0.f;
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

void AProductionsite::InitProductionSite(FProductionSiteSaveData _saveData, AMarketManager* _marketManager, APlayerBase* _siteOwner)
{
	world = GetWorld();
	productionSiteType = _saveData.GetSavedType();
	actorMesh = _saveData.GetSavedMesh();
	actorMeshComponent->SetStaticMesh(actorMesh);
	marketManager = _marketManager;
	buildingSite = _saveData.GetSavedBuildingSite();
	siteID = _saveData.GetProductionsiteID();
	siteOwner = _siteOwner;

	productionSiteResourcePool = _saveData.GetSavedResourcePool();
	productionResourceHandlePair = _saveData.GetSavedResourceHandle();

	for (TTuple<FProductionResources, FTimerHandle> resourcehandlepair : productionResourceHandlePair)
	{
		if (resourcehandlepair.Key.GetHasCost())
			TickResourceProduction(resourcehandlepair.Key.GetResourceIdent(), resourcehandlepair.Key.GetResourceCost());
		else
			TickResourceProduction(resourcehandlepair.Key.GetResourceIdent());
	}
}

void AProductionsite::InitProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite, AMarketManager* _marketManager, int _siteID, APlayerBase* _siteOwner)
{
	world = GetWorld();
	productionSiteType = _type;
	actorMesh = _siteMesh;
	actorMeshComponent->SetStaticMesh(actorMesh);
	marketManager = _marketManager;
	buildingSite = _buildingSite;
	siteID = _siteID;
	siteOwner = _siteOwner;

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
		productionSiteResourcePool,
		productionResourceHandlePair,
		productionSiteProductivity
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

// Resourcen sollen immer nur ticken solange ihr arbeiter zugewiesen wurden

// Wir loopen durch alle resourcespairs und checken on die callende resource in der lokalen map ist
// Dies mach ich damit ich eine refferenz auf die callende resource habe, welche ich dann über deren TickResource auf den neuen wert erhöhe
// Danach resette ich den timer der resource
void AProductionsite::TickResourceProduction(EResourceIdent _resourceIdent)
{
	for (TTuple<FProductionResources, FTimerHandle> resourcehandlepair : productionResourceHandlePair)
	{
		if (_resourceIdent == resourcehandlepair.Key.GetResourceIdent())
		{
			FTimerDelegate  currdelegate;
			currdelegate.BindUFunction(this, FName("TickResourceProduction"), resourcehandlepair.Key.GetResourceIdent());

			float tickrate = resourcehandlepair.Key.GetResourceTickRate() - productionSiteProductivity;
			world->GetTimerManager().SetTimer(resourcehandlepair.Value, currdelegate, tickrate, false);

			if (subscribedWorker.Num() > 0)
			{
				EResourceIdent ident = resourcehandlepair.Key.GetResourceIdent();
				productionSiteResourcePool.Add(ident, productionSiteResourcePool.FindRef(ident) + resourceStdTickValue);
			}

			break;
		}
	}
}

// _resourceIdent ist die welche produziert wird, _resourceCost representiert die kosten einer einheit
// Das ist mit abfrage für den ident um es generisch zu halten
void AProductionsite::TickResourceProduction(EResourceIdent _resourceIdent, TMap<EResourceIdent, int> _resourceCost)
{
	UE_LOG(LogTemp, Warning, TEXT("sdfsd"));

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

			if(bcanafford)
			{
				if (subscribedWorker.Num() > 0)
				{
					for (TTuple<EResourceIdent, int> item : _resourceCost)
					{
						productionSiteResourcePool.Add(item.Key, productionSiteResourcePool.FindRef(item.Key) - item.Value);
					}

					EResourceIdent ident = resourcehandlepair.Key.GetResourceIdent();
					productionSiteResourcePool.Add(ident, productionSiteResourcePool.FindRef(ident) + resourceStdTickValue);
				}
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("AProductionsite, Not enough resources to tick resource with cost"));

			EResourceIdent ident = resourcehandlepair.Key.GetResourceIdent();
			TMap<EResourceIdent, int> resourcecost = resourcehandlepair.Key.GetResourceCost();

			auto ticklambda = [this, ident, resourcecost]()
			{
				TickResourceProduction(ident, resourcecost);
			};

			FTimerDelegate  currdelegate;
			currdelegate.BindLambda(ticklambda);

			float tickrate = resourcehandlepair.Key.GetResourceTickRate() - productionSiteProductivity;

			world->GetTimerManager().SetTimer(resourcehandlepair.Value, currdelegate, tickrate, false);
		}
	}
}

void AProductionsite::SubscribeWorker(AWorker* _toSub)
{
	if (!subscribedWorker.Contains(_toSub))
	{
		subscribedWorker.Add(_toSub);

		productionSiteProductivity += _toSub->GetProductivity();
	}
	else
		UE_LOG(LogTemp,Warning,TEXT("AProductionsite, subscribedWorker.Contains(_toSub)"))
}

void AProductionsite::UnsubscribeWorker(AWorker* _toUnsub)
{
	if (subscribedWorker.Contains(_toUnsub))
	{
		subscribedWorker.Remove(_toUnsub);

		productionSiteProductivity -= _toUnsub->GetProductivity();
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, !subscribedWorker.Contains(_toUnsub)"))
}

// Ich checke welcher resourcentyp mit welcher production site übereinstimmt und adde dann das pair
// Dann werden für alle pairs die resourcetick gecalled, in ihr wird nach dem ersten tick der timer für jede resoruce gestellt
void AProductionsite::InitResources()
{
	// Resource in pool adden

	TMap<EResourceIdent, FIndividualResourceInfo> poolinfo = marketManager->GetPoolInfo();

	for(TTuple<EResourceIdent, FIndividualResourceInfo> item: poolinfo)
	{
		productionSiteResourcePool.Add(item.Key, 100);

		if (productionSiteType == item.Value.GetAllowedProductionSites())
		{
			FProductionResources productionResource =
			{
				item.Key,
				item.Value.GetResourceTickRate(),
				FString::FromInt((int)item.Value.GetResourceIdent()),
				FString::FromInt((int)item.Value.GetAllowedProductionSites()),
				item.Value.GetHasCost(),
				item.Value.GetResourceCost()
			};

			productionResourceHandlePair.Add(productionResource, FTimerHandle());
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
