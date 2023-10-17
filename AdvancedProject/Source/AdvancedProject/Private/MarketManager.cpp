
#include "MarketManager.h"
#include "MarketStall.h"
#include "ResourceTableBase.h"
#include "Engine/DataTable.h"


AMarketManager::AMarketManager()
{
	PrimaryActorTick.bCanEverTick = true;


	resourcePriceTick = 1.f;
	resourceMinValue = 1.f;
}


bool AMarketManager::InitMarketManager(FMarketManagerSaveData _saveData)
{
	bool status = false;

	UE_LOG(LogTemp, Warning, TEXT("SAVEDATA"));

	world = GetWorld();

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager, !NullcheckDependencies"));
		return false;
	}
	else
		status = true;

	InitResources(_saveData);
	InitMarketStalls();

	FTimerHandle handle;
	world->GetTimerManager().SetTimer(handle, this, &AMarketManager::UpdateResourcePrices, resourcePriceTick, false);

	return status;
}

bool AMarketManager::InitMarketManager()
{
	bool status = false;

	UE_LOG(LogTemp, Warning, TEXT("NO SAVEDATA"));

	world = GetWorld();

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager, !NullcheckDependencies"));
		return false;
	}
	else
		status = true;

	TArray<FResourceTableBase*> resourcebasevalues;

	for(TTuple<FName, unsigned char*> rowitem : resourceDefaultTable->GetRowMap())
	{
		resourcebasevalues.Add(reinterpret_cast<FResourceTableBase*>(rowitem.Value));
	}

	for (size_t i = 0; i < resourcebasevalues.Num(); i++)
	{
		InitIndividualResource(resourcebasevalues[i]->Resource.GetLastResourcePrice(), resourcebasevalues[i]->Resource.GetResourceAmount(), resourcebasevalues[i]->Resource.GetResourceIdent(), resourcebasevalues[i]->Resource.GetAllowedProductionSites(), resourcebasevalues[i]->Resource.GetResourceTickRate(),
			resourcebasevalues[i]->Resource.GetHasCost(), resourcebasevalues[i]->Resource.GetResourceCost());
	}

	InitMarketStalls();

	FTimerHandle handle;
	world->GetTimerManager().SetTimer(handle, this, &AMarketManager::UpdateResourcePrices, resourcePriceTick, false);

	return status;
}

// Noch resourcen auf dem markt auf 0 checken
// Obergrenzen müssen noch mit einfaktoriert werden
TArray<FResourceTransactionTicket> AMarketManager::BuyResources(TArray<FResourceTransactionTicket> _resourcesToBuy)
{
	TArray<FResourceTransactionTicket> returntickets;

	for(FResourceTransactionTicket ticket : _resourcesToBuy)
	{
		FResourceTransactionTicket newticketentry;

		EResourceIdent currident = ticket.resourceIdent;
		FIndividualResourceInfo resourcelistinfo = resourceList.FindRef(currident);

		if(resourcelistinfo.GetLastResourcePrice() <= ticket.maxBuyPricePerResource.GetValue())
		{
			// !! NOCH EINE LÖSUNG FÜR CALLS FINDEN WELCHE ZEITGLEICH STATTFINDEN !! -> Waiting List?
			resourceList.Find(ticket.resourceIdent)->SubtractResourceAmount(ticket.resourceAmount);
			resourceList.Find(ticket.resourceIdent)->Subtract_K_Value(ticket.resourceAmount);

			newticketentry.resourceIdent = ticket.resourceIdent;
			newticketentry.resourceAmount = ticket.resourceAmount;

			// Wir geben die differenz wieder zurück da jeder käufer immer all sein geld reinwirft
			newticketentry.exchangedCapital = ticket.exchangedCapital - ticket.resourceAmount * resourcelistinfo.GetLastResourcePrice();

			returntickets.Add(newticketentry);
		}
		else
		{
			newticketentry.resourceIdent = ticket.resourceIdent;
			newticketentry.resourceAmount = 0;
			newticketentry.exchangedCapital = ticket.exchangedCapital;

			returntickets.Add(newticketentry);
			UE_LOG(LogTemp, Warning, TEXT("AMarketManager, Resource could not be bought because the price is to high"));
		}
	}

	return returntickets;
}

// Ich glaub min brauch ich nicht?
// Untergrenzen müssen noch mit einfaktoriert werden
TArray<FResourceTransactionTicket> AMarketManager::SellResources(TArray<FResourceTransactionTicket> _resourcesToSell)
{
	TArray<FResourceTransactionTicket> returntickets;

	for (FResourceTransactionTicket ticket : _resourcesToSell)
	{
		FResourceTransactionTicket newticketentry;

		EResourceIdent currident = ticket.resourceIdent;
		FIndividualResourceInfo resourcelistinfo = resourceList.FindRef(currident);

		if (resourcelistinfo.GetLastResourcePrice() >= ticket.minSellPricePricePerResource.GetValue())
		{
			resourceList.Find(ticket.resourceIdent)->AddResourceAmount(ticket.resourceAmount);
			resourceList.Find(ticket.resourceIdent)->Add_K_Value(ticket.resourceAmount);

			newticketentry.resourceIdent = ticket.resourceIdent;
			newticketentry.resourceAmount = 0;

			// Wir geben die differenz wieder zurück da jeder käufer immer all sein geld reinwirft
			newticketentry.exchangedCapital = ticket.resourceAmount * resourcelistinfo.GetLastResourcePrice();

			returntickets.Add(newticketentry);
		}
		else
		{
			newticketentry.resourceIdent = ticket.resourceIdent;
			newticketentry.resourceAmount = ticket.resourceAmount;
			newticketentry.exchangedCapital = 0;

			returntickets.Add(newticketentry);
			UE_LOG(LogTemp, Warning, TEXT("AMarketManager, Resource could not be bought because the price is not high enough"));
		}
	}

	return returntickets;
}

void AMarketManager::InitMarketStalls()
{
	for (size_t i = 0; i < stallPositions.Num(); i++)
	{
		FVector spawnpos =  stallPositions[i];
		AMarketStall* tospawn = Cast<AMarketStall>(world->SpawnActor(marketStallClass, &spawnpos));

		if (tospawn)
		{
			tospawn->SetMesh(stallMeshes[i]);
			spawnedStalls.Add(tospawn);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("AMarketManager, !tospawn"))
	}
}

void AMarketManager::InitResources(FMarketManagerSaveData _saveData)
{
	TArray<FIndividualResourceInfo> allresources;
	resourceList.GenerateValueArray(allresources);

	for (FIndividualResourceInfo resource : allresources)
	{
		InitIndividualResource(resource.GetLastResourcePrice(), resource.GetResourceAmount(), resource.GetResourceIdent(), resource.GetAllowedProductionSites(), 
							   resource.GetResourceTickRate(), resource.GetHasCost(), resource.GetResourceCost());
	}
}

void AMarketManager::UpdateResourcePrices()
{
	TArray<FIndividualResourceInfo> allresources;
	resourceList.GenerateValueArray(allresources);

	for(FIndividualResourceInfo resource : allresources)
	{
		float lastresourceprize = resource.GetLastResourcePrice();
		float k_value = resource.Get_K_Value();
		float timeframe = resourcePriceTick;
		// Timeframe sollte unter umsatänden durch einen wert ersetzt werden welcher die meneg an zeit zwischen der letzten und dieser tranaktion darstellt
		float newPrice = lastresourceprize;


		if (k_value > 0)
			newPrice = lastresourceprize * FMath::Exp(k_value * timeframe / 10);
		else if (k_value < 0)
			newPrice = lastresourceprize * FMath::Exp(-k_value * timeframe / 10);

		if (newPrice < resourceMinValue)
			newPrice = resourceMinValue;

		resourceList.Find(resource.GetResourceIdent())->SetResourceTickRate(newPrice);
		resourceList.Find(resource.GetResourceIdent())->Set_K_Value(0.f);
	}

	/* Kauf und verkauf test zum checken ob die formel richtig funzt
	 *UE_LOG(LogTemp, Warning, TEXT("%f"), newPrice);
	TOptional<float> nomaxminprice;

	TArray<FResourceTransactionTicket> buycall =
	{
		FResourceTransactionTicket(1, 10, EResourceIdent::ERI_Gold, 100,nomaxminprice),
	};

	TArray<FResourceTransactionTicket> sellcall =
	{
		FResourceTransactionTicket(4, 10, EResourceIdent::ERI_Gold, nomaxminprice,0),
	};

	test++;

	if (test % 5 == 0)
	{
		BuyResources(buycall);
		UE_LOG(LogTemp, Warning, TEXT("BUY"));
	}

	if (test % 10 == 0)
	{
		SellResources(sellcall);
		UE_LOG(LogTemp, Warning, TEXT("SELL"));
	}*/

	FTimerHandle handle;
	world->GetTimerManager().SetTimer(handle, this, &AMarketManager::UpdateResourcePrices, resourcePriceTick, false);
}

void AMarketManager::InitIndividualResource(float _lastResourcePrice, int _resourceAmount, EResourceIdent _resourceIdent, EProductionSiteType _allowedProductionSite, float _resourceTickRate, 
											bool _bHasCost, TMap<EResourceIdent, int> _resourceCost)
{
	FIndividualResourceInfo currentresource =
	{
		_resourceIdent,
		_allowedProductionSite,
		_resourceAmount,
		_lastResourcePrice,
		0.f,
		_resourceTickRate,
		_bHasCost,
		_resourceCost
	};

	resourceList.Add(_resourceIdent, currentresource);
}

FMarketManagerSaveData AMarketManager::GetMarketManagerSaveData()
{
	FMarketManagerSaveData newsavefile = {resourceList};

	return newsavefile;
}

bool AMarketManager::NullcheckDependencies()
{
	bool status = true;

	if (stallPositions.Num() <= 0)
	{
		status = true;
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager !stallPositions"));
	}

	if (!world)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager !world"));
	}

	if (!marketStallClass)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager !marketStallClass"));
	}

	if (!resourceDefaultTable)
	{
		status = false;
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager !resourceDefaultTable"));
	}

	return status;
}
