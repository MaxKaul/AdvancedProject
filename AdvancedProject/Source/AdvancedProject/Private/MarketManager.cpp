
#include "MarketManager.h"
#include "MarketStall.h"
#include "ResourceTableBase.h"
#include "Engine/DataTable.h"


AMarketManager::AMarketManager()
{
	PrimaryActorTick.bCanEverTick = true;

}


bool AMarketManager::InitMarketManager(FMarketManagerSaveData _saveData)
{
	bool status = false;

	world = GetWorld();

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager, !NullcheckDependencies"))
			status = false;
	}
	else
		status = true;
		
	

	InitResources(_saveData);
	InitMarketStalls();

	return status;
}

bool AMarketManager::InitMarketManager(bool _noSaveData)
{
	bool status = false;

	TArray<FResourceTableBase*> resourcebasevalues;

	for(TTuple<FName, unsigned char*> rowitem : resourceDefaultTable->GetRowMap())
	{
		resourcebasevalues.Add(reinterpret_cast<FResourceTableBase*>(rowitem.Value));
	}

	for (size_t i = 0; i < resourcebasevalues.Num(); i++)
	{
		InitIndividualResource(resourcebasevalues[i]->Resource.lastResourcePrice, resourcebasevalues[i]->Resource.priceEvaluationTime, resourcebasevalues[i]->Resource.resourceAmount, resourcebasevalues[i]->Resource.resourceIdent);
	}

	return status;
}


void AMarketManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<FResourceTransactionTicket> AMarketManager::BuyResources(TArray<FResourceTransactionTicket> _resourcesToBuy)
{
	TArray<FResourceTransactionTicket> returntickets;

	for(FResourceTransactionTicket ticket : _resourcesToBuy)
	{
		FResourceTransactionTicket newticketentry;

		EResourceIdent currident = ticket.resourceIdent;
		FIndividualResourceInfo resourcelistinfo = resourceList.FindRef(currident);

		if(ticket.exchangedCapital <= ticket.resourceAmount * resourcelistinfo.lastResourcePrice)
		{
			FIndividualResourceInfo todetuct = resourceList.FindRef(ticket.resourceIdent);
			todetuct.resourceAmount -= ticket.resourceAmount;

			newticketentry.resourceIdent = ticket.resourceIdent;
			newticketentry.resourceAmount = ticket.resourceAmount;

			// Wir geben die differenz wieder zurück da jeder käufer immer all sein geld reinwirft
			newticketentry.exchangedCapital = ticket.exchangedCapital - ticket.resourceAmount * resourcelistinfo.lastResourcePrice;

			returntickets.Add(newticketentry);
		}
		else
		{
			newticketentry.resourceIdent = ticket.resourceIdent;
			newticketentry.resourceAmount = 0;
			newticketentry.exchangedCapital = ticket.resourceAmount;

			returntickets.Add(newticketentry);
			UE_LOG(LogTemp, Warning, TEXT("AMarketManager, Resource could not be bought because the prize is to high"));
		}
	}

	return returntickets;
}

TArray<FResourceTransactionTicket> AMarketManager::SellResources(TArray<FResourceTransactionTicket> _resourcesToSell)
{
	TArray<FResourceTransactionTicket> returntickets;

	return returntickets;
}

void AMarketManager::InitMarketStalls()
{
	for (size_t i = 0; i < stallPositions.Num(); i++)
	{
		AMarketStall* tospawn;
		FVector spawnpos =  stallPositions[i];
		tospawn = Cast<AMarketStall>(world->SpawnActor(marketStallClass, &spawnpos));

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

	for(TTuple<EResourceIdent, FIndividualResourceInfo> key : _saveData.resourceIdentInfoPair)
	{
		switch (key.Key)
		{
		case EResourceIdent::ERI_Gold:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceIdent);
			break;

		case EResourceIdent::ERI_Iron:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceIdent);
			break;

		case EResourceIdent::ERI_Wheat:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceIdent);
			break;

		case EResourceIdent::ERI_Meat:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceIdent);
			break;

		case EResourceIdent::ERI_Fruit:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceIdent);
			break;

		case EResourceIdent::ERI_Resin:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceIdent);
			break;

		case EResourceIdent::ERI_Jewelry:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceIdent);
			break;

		case EResourceIdent::ERI_Furniture:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceIdent);
			break;

		case EResourceIdent::ERI_Ambrosia:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
						      	   _saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
						      	   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount,
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceIdent);
			break;


		case EResourceIdent::ERI_ENTRY_AMOUNT:
			break;
		case EResourceIdent::ERI_DEFAULT:
			break;
		default:
			break;
		}

	}
}

void AMarketManager::InitIndividualResource(float _lastResourcePrice, float _priceEvaluationTime, int _resourceAmount, EResourceIdent _resourceIdent)
{
	FIndividualResourceInfo currentresource;

	currentresource.lastResourcePrice = _lastResourcePrice;
	currentresource.priceEvaluationTime = _priceEvaluationTime;
	currentresource.resourceAmount = _resourceAmount;
	currentresource.resourceIdent = _resourceIdent;

	resourceList.Add(_resourceIdent, currentresource);
}

bool AMarketManager::NullcheckDependencies()
{
	bool status = false;

	if (stallPositions.Num() > 0)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager !stallPositions"));

	if (world)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("world !stallPositions"));

	if (marketStallClass)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager !marketStall"));

	if(resourceDefaultTable)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager !marketStall"));


	return status;
}
