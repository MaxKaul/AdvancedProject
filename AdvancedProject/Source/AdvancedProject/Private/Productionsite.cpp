// Fill out your copyright notice in the Description page of Project Settings.

#include "Productionsite.h"

#include "BuildingSite.h"
#include "MarketManager.h"
#include "Worker.h"
#include "PlayerBase.h"
#include "..\Public\TableBaseLibrary.h"

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

void FPS_OverloadFuncs::InitProductionSite(FProductionSiteSaveData _saveData, AMarketManager* _marketManager, APlayerBase* _siteOwner, FPS_OverloadFuncs* _overloadFuncs, UProductionSiteManager* _productionSiteManager)
{
	overloadOwner->world = overloadOwner->GetWorld();
	overloadOwner->productionSiteType = _saveData.GetSavedType();
	overloadOwner->actorMesh = _saveData.GetSavedMesh();
	overloadOwner->actorMeshComponent->SetStaticMesh(overloadOwner->actorMesh);
	overloadOwner->marketManager = _marketManager;
	overloadOwner->buildingSite = _saveData.GetSavedBuildingSite();
	overloadOwner->siteID = _saveData.GetProductionsiteID();
	overloadOwner->siteOwner = _siteOwner;
	overloadOwner->overloadFuncs = _overloadFuncs;
	overloadOwner->productionSiteManager = _productionSiteManager;

	overloadOwner->productionSiteResourcePool = _saveData.GetSavedResourcePool();
	overloadOwner->productionResourceHandlePair = _saveData.GetSavedResourceHandle();

	for (TTuple<FProductionResources, FTimerHandle> resourcehandlepair : overloadOwner->productionResourceHandlePair)
	{
		if (resourcehandlepair.Key.GetHasCost())
			TickResourceProduction(resourcehandlepair.Key.GetResourceIdent(), resourcehandlepair.Key.GetResourceCost());
		else
			TickResourceProduction(resourcehandlepair.Key.GetResourceIdent());
	}
}

void FPS_OverloadFuncs::InitProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite, AMarketManager* _marketManager, int _siteID, APlayerBase* _siteOwner, 
										   FPS_OverloadFuncs* _overloadFuncs, UProductionSiteManager* _productionSiteManager)
{
	overloadOwner->world = overloadOwner->GetWorld();
	overloadOwner->productionSiteType = _type;
	overloadOwner->actorMesh = _siteMesh;
	overloadOwner->actorMeshComponent->SetStaticMesh(overloadOwner->actorMesh);
	overloadOwner->marketManager = _marketManager;
	overloadOwner->buildingSite = _buildingSite;
	overloadOwner->siteID = _siteID;
	overloadOwner->siteOwner = _siteOwner;
	overloadOwner->overloadFuncs = _overloadFuncs;
	overloadOwner->productionSiteManager = _productionSiteManager;

	overloadOwner->InitResources();
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

// Ich setzte von hier aus gleichzeitig die daten im data table und schicke eine ref vom ident zum UI, muss noch nagepasst werden
void AProductionsite::GetDisplayInfo( FProductionSiteDisplayInfos& _displayInfo)
{
	TMap<EProductionSiteType, UDataTable*> newdisplayinfo;

	TArray<FProductionSiteInfoBase*> displaybase;

	for (TTuple<FName, unsigned char*> rowitem : productionSideDisplayTable->GetRowMap())
	{
		displaybase.Add(reinterpret_cast<FProductionSiteInfoBase*>(rowitem.Value));
	}

	for (FProductionSiteInfoBase* base : displaybase)
	{
		if (base->productionSiteDisplayInfos.productionSiteType == productionSiteType)
		{
			FProductionSiteDisplayInfos sitedisplayinfo;
		
			sitedisplayinfo = base->productionSiteDisplayInfos;
			sitedisplayinfo.workerAmount = subscribedWorker.Num();
			sitedisplayinfo.resourceOutput = resourceStdTickValue;
			
			_displayInfo = sitedisplayinfo;
		
			break;
		}
	}
}

// Resourcen sollen immer nur ticken solange ihr arbeiter zugewiesen wurden

// Wir loopen durch alle resourcespairs und checken on die callende resource in der lokalen map ist
// Dies mach ich damit ich eine refferenz auf die callende resource habe, welche ich dann über deren TickResource auf den neuen wert erhöhe
// Danach resette ich den timer der resource
void FPS_OverloadFuncs::TickResourceProduction(EResourceIdent _resourceIdent)
{
	for (TTuple<FProductionResources, FTimerHandle> resourcehandlepair : overloadOwner->productionResourceHandlePair)
	{
		if (_resourceIdent != resourcehandlepair.Key.GetResourceIdent())
			continue;

		EResourceIdent timerident = resourcehandlepair.Key.GetResourceIdent();

		auto ticklambda = [this, timerident]()
		{
			TickResourceProduction(timerident);
		};

		FTimerDelegate  currdelegate;
		currdelegate.BindLambda(ticklambda);

		float tickrate = resourcehandlepair.Key.GetResourceTickRate() - overloadOwner->productionSiteProductivity;

		overloadOwner->world->GetTimerManager().SetTimer(resourcehandlepair.Value, currdelegate, tickrate, false);

		if (overloadOwner->subscribedWorker.Num() <= 0)
			return;

		EResourceIdent ident = resourcehandlepair.Key.GetResourceIdent();
		overloadOwner->productionSiteResourcePool.Add(ident, overloadOwner->productionSiteResourcePool.FindRef(ident) + overloadOwner->resourceStdTickValue);

		break;
	}
}

// _resourceIdent ist die welche produziert wird, _resourceCost representiert die kosten einer einheit
// Das ist mit abfrage für den ident um es generisch zu halten
void FPS_OverloadFuncs::TickResourceProduction(EResourceIdent _resourceIdent, TMap<EResourceIdent, int> _resourceCost)
{
	for (TTuple<FProductionResources, FTimerHandle> resourcehandlepair : overloadOwner->productionResourceHandlePair)
	{
		if (_resourceIdent != resourcehandlepair.Key.GetResourceIdent())
			continue;
			
		bool bcanafford = false;
		int index = 0;

		for (TTuple<EResourceIdent, int> item : _resourceCost)
		{
			index++;

			if (overloadOwner->productionSiteResourcePool.FindRef(item.Key) < item.Value)
				return;

			if (index >= _resourceCost.Num())
				bcanafford = true;
		}

		if (!bcanafford)
			return;

		if (overloadOwner->subscribedWorker.Num() <= 0)
			return;

		for (TTuple<EResourceIdent, int> item : _resourceCost)
		{
			overloadOwner->productionSiteResourcePool.Add(item.Key, overloadOwner->productionSiteResourcePool.FindRef(item.Key) - item.Value);
		}

		EResourceIdent ident = resourcehandlepair.Key.GetResourceIdent();
		overloadOwner->productionSiteResourcePool.Add(ident, overloadOwner->productionSiteResourcePool.FindRef(ident) + overloadOwner->resourceStdTickValue);

		TMap<EResourceIdent, int> resourcecost = resourcehandlepair.Key.GetResourceCost();

		auto ticklambda = [this, ident, resourcecost]()
		{
			TickResourceProduction(ident, resourcecost);
		};

		FTimerDelegate  currdelegate;
		currdelegate.BindLambda(ticklambda);

		float tickrate = resourcehandlepair.Key.GetResourceTickRate() - overloadOwner->productionSiteProductivity;

		overloadOwner->world->GetTimerManager().SetTimer(resourcehandlepair.Value, currdelegate, tickrate, false);
	}
}

void AProductionsite::SubscribeWorker(AWorker* _toSub)
{
	if (!_toSub)
	{
		if (productionSiteManager->GetAllUnasignedWorker().Num() <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("AProductionsite, productionSiteManager->GetAllLocalWorker().Num() <= 0"))
				return;
		}

		TArray<AWorker*> availableworker = productionSiteManager->GetAllUnasignedWorker();
		int32 workerid = FMath::RandRange(0,availableworker.Num() - 1);
		
		if(availableworker[workerid] && availableworker.Contains(availableworker[workerid]))
			_toSub = availableworker[workerid];
	}


	if (!subscribedWorker.Contains(_toSub))
	{
		subscribedWorker.Add(_toSub);
		productionSiteProductivity += _toSub->GetProductivity();

		productionSiteManager->UnsubscribeWorkerToProductionSite(_toSub, this);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("AProductionsite, subscribedWorker.Contains(_toSub)"));
}

void AProductionsite::UnsubscribeWorker(AWorker* _toUnsub, bool _fromManager)
{
	if(!_toUnsub)
	{
		if(subscribedWorker.Num() <= 0)
		{
			UE_LOG(LogTemp,Warning,TEXT("AProductionsite, subscribedWorker.Num() <= 0"))
			return;
		}

		int32 workerid = FMath::RandRange(0, subscribedWorker.Num() -1);

		if (subscribedWorker[workerid] && subscribedWorker.Contains(subscribedWorker[workerid]))
			_toUnsub = subscribedWorker[workerid];
	}

	if (subscribedWorker.Contains(_toUnsub))
	{
		subscribedWorker.Remove(_toUnsub);
		productionSiteProductivity -= _toUnsub->GetProductivity();

		if(!_fromManager)
			productionSiteManager->SubscribeWorkerToLocalPool(_toUnsub, this, true);
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
				overloadFuncs->TickResourceProduction(resourcehandlepair.Key.GetResourceIdent(), resourcehandlepair.Key.GetResourceCost());
			else
				overloadFuncs->TickResourceProduction(resourcehandlepair.Key.GetResourceIdent());
		}
	}
	else
		UE_LOG(LogTemp,Warning,TEXT("AProductionsite, productionResource.Num() < 0"))
}
