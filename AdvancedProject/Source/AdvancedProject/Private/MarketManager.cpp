
#include "MarketManager.h"
#include "MarketStall.h"
#include "ParameterCollection.h"
#include "..\Public\TableBaseLibrary.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetArrayLibrary.h"


AMarketManager::AMarketManager()
{
	PrimaryActorTick.bCanEverTick = true;

	resourceMinValue = 1.f;
}

void FMM_OverloadFuncs::InitMarketManager(FMarketManagerSaveData _saveData)
{
	UE_LOG(LogTemp, Warning, TEXT("InitMarketManager with SAVEDATA"));

	overloadOwner->world = overloadOwner->GetWorld();

	if (!overloadOwner->NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager, !NullcheckDependencies"));
		return;
	}

	overloadOwner->InitResources(_saveData);
	overloadOwner->InitMarketStalls(_saveData.GetMarketStallSaveData());
}

void FMM_OverloadFuncs::InitMarketManager()
{
	UE_LOG(LogTemp, Warning, TEXT("NO SAVEDATA"));

	overloadOwner->world = overloadOwner->GetWorld();

	if (!overloadOwner->NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager, !NullcheckDependencies"));
		return;
	}

	TArray<FResourceTableBase*> resourcebasevalues;

	for (TTuple<FName, unsigned char*> rowitem : overloadOwner->resourceDefaultTable->GetRowMap())
	{
		resourcebasevalues.Add(reinterpret_cast<FResourceTableBase*>(rowitem.Value));
	}

	for (size_t i = 0; i < resourcebasevalues.Num(); i++)
	{
		overloadOwner->InitIndividualResource(resourcebasevalues[i]->Resource.GetLastResourcePrice(), resourcebasevalues[i]->Resource.GetResourceAmount(), resourcebasevalues[i]->Resource.GetResourceIdent(), 
			resourcebasevalues[i]->Resource.GetAllowedProductionSites(), resourcebasevalues[i]->Resource.GetLastUpdated(),
			resourcebasevalues[i]->Resource.GetHasCost(), resourcebasevalues[i]->Resource.GetResourceCost(), resourcebasevalues[i]->Resource.GetFulfillmentValue());
	}

	TArray<FMarketStallSaveData> newstallsave = overloadOwner->GenerateStallTypes();

	overloadOwner->InitMarketStalls(newstallsave);
}


TArray<FMarketStallSaveData> AMarketManager::GenerateStallTypes()
{
	TArray<FMarketStallSaveData> stallsave;

	TArray<EResourceIdent> allresources;
	TArray<int32> shufflearray;

	TArray<EResourceIdent> usedresources;

	for (size_t j = 1; j < (int)EResourceIdent::ERI_ENTRY_AMOUNT; j++)
	{
		allresources.Add(EResourceIdent(j));
	}

	if (allresources.Num() > 0)
	{
		int32 LastIndex = allresources.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				allresources.Swap(i, Index);
			}
		}
	}

	for (size_t i = 0; i < stallPositions.Num(); i++)
	{
		FMarketStallSaveData newsave;
	
		if(i <= allresources.Num() )
		{
			newsave.AddNewProductionType_S(allresources[i]);
			usedresources.Add(allresources[i]);
		}
	
		if (i + 2 < allresources.Num())
		{
			newsave.AddNewProductionType_S(allresources[i + 2]);
			usedresources.Add(allresources[i + 2]);
		}
	
		if (i + 3 < allresources.Num())
		{
			newsave.AddNewProductionType_S(allresources[i + 3]);
			usedresources.Add(allresources[i + 3]);
		}
	
		// Zum catchen von edgecases falls eine resource nicht richtig zugewiesen wurde
		if(i >= stallPositions.Num() - 1)
		{
			for(EResourceIdent ident : allresources)
			{
				if(!usedresources.Contains(ident))
				{
					newsave.AddNewProductionType_S(ident);
					usedresources.Add(ident);
				}
			}
		}
		newsave.SetStallID(i);
		stallsave.Add(newsave);
	}

	return stallsave;
}

// Noch resourcen auf dem markt auf 0 checken
// Obergrenzen müssen noch mit einfaktoriert werden
TArray<FResourceTransactionTicket> AMarketManager::BuyResources(TArray<FResourceTransactionTicket> _transactionOrder)
{
	TArray<FResourceTransactionTicket> returntickets;

	for(FResourceTransactionTicket ticket : _transactionOrder)
	{
		FResourceTransactionTicket newticketentry;

		EResourceIdent currident = ticket.resourceIdent;
		FIndividualResourceInfo resourcelistinfo = resourceList.FindRef(currident);

		int resourceamount = ticket.resourceAmount;

		if(resourcelistinfo.GetLastResourcePrice() <= ticket.maxBuyPricePerResource && resourcelistinfo.GetResourceAmount() > 0)
		{
			if (resourceamount > resourcelistinfo.GetResourceAmount())
				resourceamount = resourcelistinfo.GetResourceAmount();

			if(ticket.exchangedCapital < resourcelistinfo.GetLastResourcePrice() * ticket.resourceAmount)
				resourceamount = ticket.exchangedCapital / resourcelistinfo.GetLastResourcePrice();

			if(resourceamount > 0)
			{
				resourceList.Find(ticket.resourceIdent)->SubtractResourceAmount(resourceamount);
				resourceList.Find(ticket.resourceIdent)->Subtract_K_Value(resourceamount);

				newticketentry.resourceIdent = ticket.resourceIdent;
				newticketentry.resourceAmount = resourceamount;

				newticketentry.exchangedCapital = ticket.exchangedCapital - resourceamount * resourcelistinfo.GetLastResourcePrice();

				returntickets.Add(newticketentry);
			}
			else
			{
				newticketentry.resourceIdent = ticket.resourceIdent;
				newticketentry.resourceAmount = 0;
				newticketentry.exchangedCapital = ticket.exchangedCapital;
			}

		}
		else
		{
			newticketentry.resourceIdent = ticket.resourceIdent;
			newticketentry.resourceAmount = 0;
			newticketentry.exchangedCapital = ticket.exchangedCapital;

			returntickets.Add(newticketentry);

			UE_LOG(LogTemp, Warning, TEXT("AMarketManager, Resource could not be bought because the price is to high or there are no resources"));
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

		if (resourcelistinfo.GetLastResourcePrice() >= ticket.minSellPricePerResource)
		{
			resourceList.Find(ticket.resourceIdent)->AddResourceAmount(ticket.resourceAmount);
			resourceList.Find(ticket.resourceIdent)->Add_K_Value(ticket.resourceAmount);

			newticketentry.resourceIdent = ticket.resourceIdent;
			newticketentry.resourceAmount = 0;

			// Wir geben die differenz wieder zurück da jeder käufer immer all sein geld reinwirft
			newticketentry.exchangedCapital = ticket.resourceAmount * resourcelistinfo.GetLastResourcePrice();

			returntickets.Add(newticketentry);

			UpdateResourcePrice(resourcelistinfo);
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

void AMarketManager::InitMarketStalls(TArray<FMarketStallSaveData> _stallSaveData)
{
	for (size_t i = 0; i < stallPositions.Num(); i++)
	{
		FVector spawnpos =  stallPositions[i];
		AMarketStall* tospawn = Cast<AMarketStall>(world->SpawnActor(marketStallClass, &spawnpos));

		if (tospawn)
		{
			if(i <= _stallSaveData.Num() - 1)
				tospawn->InitMarketStall(stallMeshes[i], _stallSaveData[i]);
			else
				tospawn->InitMarketStall(stallMeshes[i], _stallSaveData[0]);

			spawnedStalls.Add(tospawn);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("AMarketManager, !tospawn"))
	}


	auto transactionlambda = [this ]()
	{
		TickResources();
	};

	FTimerDelegate  currdelegate;
	currdelegate.BindLambda(transactionlambda);

	FTimerHandle newhandle;

	world->GetTimerManager().SetTimer(newhandle, currdelegate, 1.f, false);
}

void AMarketManager::InitResources(FMarketManagerSaveData _saveData)
{
	TArray<FIndividualResourceInfo> allresources;
	_saveData.GetResourceInfoPair().GenerateValueArray(allresources);

	for (FIndividualResourceInfo resource : allresources)
	{
		InitIndividualResource(resource.GetLastResourcePrice(), resource.GetResourceAmount(), resource.GetResourceIdent(), resource.GetAllowedProductionSites(), 
							   resource.GetLastUpdated(), resource.GetHasCost(), resource.GetResourceCost(), resource.GetFulfillmentValue());
	}
}

void AMarketManager::UpdateResourcePrice(FIndividualResourceInfo _resourceToUpdate)
{
	float lastresourceprice = _resourceToUpdate.GetLastResourcePrice();
	float k_value = _resourceToUpdate.Get_K_Value();
	float timeframe = _resourceToUpdate.GetLastUpdated();
	float newPrice = lastresourceprice;


	if (k_value > 0)
		newPrice = lastresourceprice * FMath::Exp(k_value * timeframe);
	else if (k_value < 0)
		newPrice = lastresourceprice * FMath::Exp(-k_value * timeframe);

	if (newPrice < resourceMinValue)
		newPrice = resourceMinValue;


	resourceList.Find(_resourceToUpdate.GetResourceIdent())->SetLastResourcePrice(newPrice);
	resourceList.Find(_resourceToUpdate.GetResourceIdent())->SetLastUpdated(0.f);
	resourceList.Find(_resourceToUpdate.GetResourceIdent())->Set_K_Value(0.f);
}

void AMarketManager::TickResources()
{
	for(TTuple<EResourceIdent, FIndividualResourceInfo> resource : resourceList)
	{
		resourceList.Find(resource.Key)->TickLastUpdated();
	}
}

void AMarketManager::InitIndividualResource(float _lastResourcePrice, int _resourceAmount, EResourceIdent _resourceIdent, EProductionSiteType _allowedProductionSite, float _resourceTickRate, 
                                            bool _bHasCost, TMap<EResourceIdent, int> _resourceCost, float _desirefulfillmentValue)
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
		_resourceCost,
		_desirefulfillmentValue
	};

	resourceList.Add(_resourceIdent, currentresource);
}

FMarketManagerSaveData AMarketManager::GetMarketManagerSaveData()
{
	TArray<FMarketStallSaveData> marketStallSaveData;

	for (AMarketStall* stall : spawnedStalls)
	{
		marketStallSaveData.Add(stall->GetStallSaveData());
	}

	FMarketManagerSaveData newsavefile = {resourceList, marketStallSaveData};

	return newsavefile;
}

AMarketStall* AMarketManager::GetStallByID(int _IDToFind)
{
	AMarketStall* foundstall = spawnedStalls[0];

	for(AMarketStall* stall : spawnedStalls)
	{
		if(stall->GetStallID() == _IDToFind)
		{
			foundstall = stall;
			break;
		}
	}

	return foundstall;
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
