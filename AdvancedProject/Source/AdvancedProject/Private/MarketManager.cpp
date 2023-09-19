
#include "MarketManager.h"
#include "MarketStall.h"


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

	for (size_t i = 0; i < (int)EResourceIdent::ERI_ENTRY_AMOUNT - 1; i++)
	{
		InitIndividualResource(0, 0, 0);
	}

	return status;
}


void AMarketManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
								   _saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount);
			break;

		case EResourceIdent::ERI_Iron:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
				_saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
				_saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount);
			break;

		case EResourceIdent::ERI_Wheat:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
				_saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
				_saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount);
			break;

		case EResourceIdent::ERI_Meat:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
				_saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
				_saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount);
			break;

		case EResourceIdent::ERI_Fruit:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
				_saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
				_saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount);
			break;

		case EResourceIdent::ERI_Resin:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
				_saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
				_saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount);
			break;

		case EResourceIdent::ERI_Jewelry:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
				_saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
				_saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount);
			break;

		case EResourceIdent::ERI_Furniture:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
				_saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
				_saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount);
			break;

		case EResourceIdent::ERI_Ambrosia:
			InitIndividualResource(_saveData.resourceIdentInfoPair.Find(key.Key)->lastResourcePrice,
				_saveData.resourceIdentInfoPair.Find(key.Key)->priceEvaluationTime,
				_saveData.resourceIdentInfoPair.Find(key.Key)->resourceAmount);
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

void AMarketManager::InitIndividualResource(float _lastResourcePrice, float _priceEvaluationTime, int _resourceAmount)
{
	FIndividualResourceInfo currentresource;

	currentresource.lastResourcePrice = _lastResourcePrice;
	currentresource.priceEvaluationTime = _priceEvaluationTime;
	currentresource.resourceAmount = _resourceAmount;

	resourceList.Add(currentresource);
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

	return status;
}
