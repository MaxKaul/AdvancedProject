// Fill out your copyright notice in the Description page of Project Settings.


#include "TransportManager.h"

#include "Productionsite.h"

// Sets default values for this component's properties
UTransportManager::UTransportManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}



FTransportManagerSaveData UTransportManager::GetTransportManagerSaveData()
{
	FTransportManagerSaveData newsave = 
	{
		allTransportOrders
	};

	return newsave;
}


// Called every frame
void UTransportManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void FTM_OverloadFuncs::InitTransportManager(AMarketManager* _marketManager, UProductionSiteManager* _prodSiteManager)
{
	overloadOwner->marketManager = _marketManager;
	overloadOwner->productionSiteManager = _prodSiteManager;


	overloadOwner->world = overloadOwner->GetWorld();
}

void FTM_OverloadFuncs::InitTransportManager(FTransportManagerSaveData _saveData, AMarketManager* _marketManager, UProductionSiteManager* _prodSiteManager)
{
	overloadOwner->marketManager = _marketManager;
	overloadOwner->productionSiteManager = _prodSiteManager;

	overloadOwner->world = overloadOwner->GetWorld();

	for(TTuple<FTransportOrder, float> pair : _saveData.GetTransactrionTravelTimePair_S())
	{
		//if(pair.Key.GetGoalIsMarket())
		//{
		//	TArray<AMarketStall*> possibleStalls = overloadOwner->marketManager->GetAllMarketStalls();
		//
		//	int rndidx = FMath::RandRange(0, possibleStalls.Num());
		//
		//	AMarketStall* goalStall = possibleStalls[rndidx];
		//
		//	//CreateTransportOrder(pair.Key.GetTransactionOrder(), goalStall, pair.Value);
		//}
		
	}
}

void FTM_OverloadFuncs::CreateTransportOrder(TArray<FResourceTransactionTicket> _transaction, FVector _startLocation, float _transitSpeed, bool _bGoalIsMarket, AProductionSite* _originSite, AProductionSite*, TOptional<AProductionSite*> _productionSite)
{
	FTransportOrder neworder;

	//neworder.InitOrder()
}
