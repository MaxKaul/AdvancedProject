
#include "MarketManager.h"
#include "MarketStall.h"
#include "ParameterCollection.h"
#include "..\Public\TableBaseLibrary.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"


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
			resourcebasevalues[i]->Resource.GetHasCost(), resourcebasevalues[i]->Resource.GetResourceCost(), resourcebasevalues[i]->Resource.GetFulfillmentValue(), resourcebasevalues[i]->Resource.GetDelta(),
			resourcebasevalues[i]->Resource.GetAlpha(), resourcebasevalues[i]->Resource.GetLambda());
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

		int resourcestobuy = ticket.resourceAmount;

		if(resourcelistinfo.GetLastResourcePrice() <= ticket.maxBuyPricePerResource && resourcelistinfo.GetResourceAmount() > 0)
		{
			if (resourcestobuy > resourcelistinfo.GetResourceAmount())
				resourcestobuy = resourcelistinfo.GetResourceAmount();


			if(ticket.exchangedCapital < resourcelistinfo.GetLastResourcePrice() * ticket.resourceAmount)
				resourcestobuy = ticket.exchangedCapital / resourcelistinfo.GetLastResourcePrice();

			if(resourcestobuy > 0)
			{
				newticketentry.exchangedCapital = ticket.exchangedCapital;
				newticketentry.resourceAmount = 0;
				newticketentry.resourceIdent = ticket.resourceIdent;

				for (size_t i = 0; i < resourcestobuy; i++)
				{
					float amountonmarket = resourcelistinfo.GetResourceAmount();

					resourceList.Find(ticket.resourceIdent)->UpdateDelta(1.f + (ticket.resourceAmount - 1) / amountonmarket);


					resourceList.Find(ticket.resourceIdent)->SubtractResourceAmount(1);

					newticketentry.resourceAmount += 1;

					newticketentry.exchangedCapital -= resourcelistinfo.GetLastResourcePrice();

					UpdateResourcePrice(resourcelistinfo, false);

					// Break condition after each iterartion to check wether or not there are stilol enough resources and the price aint to high
					if (resourcelistinfo.GetResourceAmount() <= 0)
						break;
					else if (ticket.exchangedCapital < resourcelistinfo.GetLastResourcePrice() * ticket.resourceAmount - i)
						break;
				}

			}
			else
			{
				newticketentry.resourceIdent = ticket.resourceIdent;
				newticketentry.resourceAmount = 0;
				newticketentry.exchangedCapital = ticket.exchangedCapital;
			}

			returntickets.Add(newticketentry);
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

		resourceList.Find(ticket.resourceIdent)->AddResourceAmount(ticket.resourceAmount);
		newticketentry.resourceIdent = ticket.resourceIdent;

		newticketentry.resourceAmount = 0;
		newticketentry.exchangedCapital = 0;

		for (size_t i = 0; i < ticket.resourceAmount; i++)
		{
			float amountonmarket = resourceList.Find(ticket.resourceIdent)->GetResourceAmount();

			resourceList.Find(ticket.resourceIdent)->UpdateDelta(1.f + (ticket.resourceAmount - 1) / amountonmarket);

			newticketentry.exchangedCapital += resourcelistinfo.GetLastResourcePrice();

			UpdateResourcePrice(resourcelistinfo, true);
		}

		// Der markt hat keine currency pouch, resourcen sellen besitzt dementsprechend noch keinen check in dieser richtung
		returntickets.Add(newticketentry);

		/* Ich benutze hier minSellPricePerResource, ich will das aber erstmal nicht benutzen
		if (resourcelistinfo.GetLastResourcePrice() >= ticket.minSellPricePerResource)
		{
			resourceList.Find(ticket.resourceIdent)->AddResourceAmount(ticket.resourceAmount);
			newticketentry.resourceIdent = ticket.resourceIdent;

			newticketentry.resourceAmount = 0;
			newticketentry.exchangedCapital = 0;

			for (size_t i = 0; i < ticket.resourceAmount; i++)
			{
				float amountonmarket = resourceList.Find(ticket.resourceIdent)->GetResourceAmount();

				resourceList.Find(ticket.resourceIdent)->UpdateDelta(1.f + (ticket.resourceAmount - 1) / amountonmarket);

				newticketentry.exchangedCapital += resourcelistinfo.GetLastResourcePrice();

				//UpdateResourcePrice(resourcelistinfo, true);
			}

			// Der markt hat keine currency pouch, resourcen sellen besitzzt dementsprechend noch keinen check in dieser richtung
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

		*/
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

	world->GetTimerManager().SetTimer(newhandle, currdelegate, 1.f, true);
}

void AMarketManager::InitResources(FMarketManagerSaveData _saveData)
{
	TArray<FIndividualResourceInfo> allresources;
	_saveData.GetResourceInfoPair().GenerateValueArray(allresources);

	for (FIndividualResourceInfo resource : allresources)
	{
		InitIndividualResource(resource.GetLastResourcePrice(), resource.GetResourceAmount(), resource.GetResourceIdent(), resource.GetAllowedProductionSites(), 
							   resource.GetLastUpdated(), resource.GetHasCost(), resource.GetResourceCost(), resource.GetFulfillmentValue(), 
							   resource.GetDelta(), resource.GetAlpha(), resource.GetLambda());
	}
}

void AMarketManager::UpdateResourcePrice(FIndividualResourceInfo _resourceToUpdate, bool _sell)
{
	float lastresourceprice = resourceList.Find(_resourceToUpdate.GetResourceIdent())->GetLastResourcePrice();
	float timeframe = resourceList.Find(_resourceToUpdate.GetResourceIdent())->GetLastUpdated();
	float resourceamount = resourceList.Find(_resourceToUpdate.GetResourceIdent())->GetResourceAmount();
	
	float demanddelta = resourceList.Find(_resourceToUpdate.GetResourceIdent())->GetDelta();
	float demandalpha = resourceList.Find(_resourceToUpdate.GetResourceIdent())->GetAlpha();
	float demandlamdba = resourceList.Find(_resourceToUpdate.GetResourceIdent())->GetLambda();

	float newPrice = lastresourceprice;

	// This formula combines the supply and demand equilibrium model with an exponential decay term based on the time since the last transaction with the
	// addition of a dynamic delta value on the basis of eachs transaction amount change
	float decayterm = FMath::Exp(-demandlamdba * timeframe);

	if(_sell)
		newPrice = lastresourceprice + demandalpha * (demanddelta - resourceamount) * decayterm;
	else
		newPrice = lastresourceprice - demandalpha * (demanddelta - resourceamount) * decayterm;


	if (newPrice < resourceMinValue)
		newPrice = resourceMinValue;

	resourceList.Find(_resourceToUpdate.GetResourceIdent())->SetLastResourcePrice(newPrice);
	resourceList.Find(_resourceToUpdate.GetResourceIdent())->SetLastUpdated(0.f);
}

void AMarketManager::TickResources()
{
	for(TTuple<EResourceIdent, FIndividualResourceInfo> resource : resourceList)
	{
		resourceList.Find(resource.Key)->TickLastUpdated();
	}
}

void AMarketManager::InitIndividualResource(float _lastResourcePrice, int _resourceAmount, EResourceIdent _resourceIdent, EProductionSiteType _allowedProductionSite, float _resourceTickRate, 
                                            bool _bHasCost, TMap<EResourceIdent, int> _resourceCost, float _desirefulfillmentValue, float _delta, float _alpha, float _lambda)
{
	if(_delta <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("_resourceDemandValue <= 0"));
		_delta = 250.f;
	}

	FIndividualResourceInfo currentresource =
	{
		_resourceIdent,
		_allowedProductionSite,
		_resourceAmount,
		_lastResourcePrice,
		_resourceTickRate,
		_bHasCost,
		_resourceCost,
		_desirefulfillmentValue,
		_delta,
		_alpha,
		_lambda
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
