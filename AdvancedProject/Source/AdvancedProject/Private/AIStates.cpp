// Fill out your copyright notice in the Description page of Project Settings.


#include "AIStates.h"

#include "AIPlayer.h"

UAIStates::UAIStates()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAIStates::BeginPlay()
{
	Super::BeginPlay();
}

void UAIStates::InitState(AAIPlayer* _owner)
{
	stateOwner = _owner;
	world = GetWorld();
}

void UAIStates::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAIStates::State_Wait()
{

}

bool UAIStates::State_BuyResources()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_BuyResources"));

	return status;
}

bool UAIStates::State_SellResources()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_SellResources"));

	return status;
}

bool UAIStates::State_TransportResources()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_TransportResources"));

	return status;
}

bool UAIStates::State_BuildSite()
{
	bool status = true;

	world->GetTimerManager().SetTimer(stateOwner->buildCooldownHandle, stateOwner->currentBehaviourBase.GetSiteConstructionCooldown(), false);

	return status;
}

bool UAIStates::State_FireWorker()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_FireWorker"));

	return status;
}

bool UAIStates::State_HireWorker()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_HireWorker"));

	return status;
}

bool UAIStates::State_AssignWorker()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_AssignWorker"));

	return status;
}

bool UAIStates::State_UnassignWorker()
{
	bool status = true;

	UE_LOG(LogTemp, Warning, TEXT("State_UnassignWorker"));

	return status;
}



// Ich will mal schaeun das ich meine tickets auch immer neu generate
// Ich will immer schauen das ich meine calculated order zurück gebe falls ich das noch für irgendwas gebrauchen kann, die sample validity ist dann in ambängigkeit ddes bools
// Dann hab ich da auch nie null drinne wenn das spiel einmal läuft
// Dies *Kann* halt auch zu bugs führen da ich auch invalide orders sample, ABER es könnte auch dazu führen das miss states sich von selbst lösen
void UAIStates::SampleBuyResources()
{
	if (stateOwner->productionSiteManager->GetAllProductionSites().Num() <= 0 || stateOwner->playerCurrency <= 0)
	{
		if (stateOwner->validStatesToTick.Contains(EAIStates::AIS_BuyResources))
			stateOwner->validStatesToTick.Remove(EAIStates::AIS_BuyResources);
	}
	else
	{
		if (!stateOwner->validStatesToTick.Contains(EAIStates::AIS_BuyResources))
			stateOwner->validStatesToTick.Add(EAIStates::AIS_BuyResources);


		TArray<FResourceTransactionTicket> calculatedOrder = CalculateBuyOrder();

		if (calculatedOrder.Num() <= 0)
		{
			if (stateOwner->validStatesToTick.Contains(EAIStates::AIS_BuyResources))
				stateOwner->validStatesToTick.Remove(EAIStates::AIS_BuyResources);

			stateOwner->sampleResult_BuyResources = FStateStatusTicket_BuyResources(false, calculatedOrder);
		}
		else
			stateOwner->sampleResult_BuyResources = FStateStatusTicket_BuyResources(true, calculatedOrder);
	}
}

// Eine BuyAction ist EINE action, bedeutet ich kaufe resourcen für EINE Site aber rechne im vorhinein aus welche möglichen order ich für alle sites haben kann
// Und nehme daruas dann ein zufällige
// -> Ich muss für das kaufen ja auch noch in betracht ziehen das eine prod site unter umständen keine resourcen benötigt, sondern nur worker zum produzieren
TArray<FResourceTransactionTicket> UAIStates::CalculateBuyOrder()
{
	TArray<AProductionsite*> allsite = stateOwner->productionSiteManager->GetAllProductionSites();
	TMap<EResourceIdent, FIndividualResourceInfo> marketpoolinfo = stateOwner->marketManager->GetPoolInfo();
	TArray<FResourceTransactionTicket> returnorder;

	// Das ist ja grade auch immer nur ein ticket prod site einrag, es können aber mehr pro site sein, also sollte ich das in ein struct *yay* wrappen
	TMap<AProductionsite*, FPossibleBuyOrders> possiblesiteorderpairs;

	float expendituredivider = stateOwner->currentBehaviourBase.GetExpenditureDivider();
	float availablecurrency = stateOwner->playerCurrency;

	for (AProductionsite* site : allsite)
	{
		if (availablecurrency <= 0)
			break;

		// The resource ident cost pair of the resources that actually have to be bought after calculations
		TMap<EResourceIdent, int> resourcestobuy;

		TArray<FProductionResources> prodresources = site->GetCurrentResources();
		TMap<EResourceIdent, int> prodsitepoolinfo = site->GetProductionSitePoolInfo();

		// Ich schaue, ja, das mach ich
		for (FProductionResources resource : prodresources)
		{
			if (!resource.GetHasCost())
				continue;

			int resourceamount = prodsitepoolinfo.FindRef(resource.GetResourceIdent());

			if (resourceamount > stateOwner->currentBehaviourBase.GetResourceAmountGoal())
				continue;

			resourcestobuy.Add(resource.GetResourceIdent(), stateOwner->currentBehaviourBase.GetResourceAmountGoal() - resourceamount);
		}

		// Wenn auf dieser site keine resourcen benötigt werden, continue zur nächsten
		if (resourcestobuy.Num() <= 0)
			continue;

		// Value = Amount
		for (TTuple<EResourceIdent, int> buyresource : resourcestobuy)
		{
			if (marketpoolinfo.Find(buyresource.Key)->GetResourceAmount() <= 0)
				continue;

			float lastprice = marketpoolinfo.Find(buyresource.Key)->GetLastResourcePrice();

			// Hierdurch sollte ich eign nichtmehr checken müssen ob ich genügend money habe im anschluss
			float possibleexpenditure = availablecurrency / expendituredivider;

			int amounttobuy = possibleexpenditure / lastprice;

			if (amounttobuy <= 0)
				continue;

			if (amounttobuy >= marketpoolinfo.Find(buyresource.Key)->GetResourceAmount())
				amounttobuy = marketpoolinfo.Find(buyresource.Key)->GetResourceAmount();

			if (amounttobuy >= buyresource.Value)
				amounttobuy = buyresource.Value;

			// Jetzt habe ich eign meinen amount to buy u. die menge an currency welche mitgegebn werden muss
			availablecurrency -= possibleexpenditure;

			FResourceTransactionTicket newticket =
			{
				amounttobuy,
				possibleexpenditure,
				buyresource.Key,
				99999,
				0
			};

			FPossibleBuyOrders possiblebuyorder;

			possiblebuyorder.AddNewTicket(newticket);

			possiblesiteorderpairs.Add(site, possiblebuyorder);
		}
	}

	if (possiblesiteorderpairs.Num() <= 0)
		UE_LOG(LogTemp, Warning, TEXT("AAIPlayer ,possiblesiteorderpairs.Num() <= 0"));

	int rnd = FMath::RandRange(0, possiblesiteorderpairs.Num());

	for (TTuple<AProductionsite*, FPossibleBuyOrders> orderpair : possiblesiteorderpairs)
	{
		if (rnd <= 0)
		{
			returnorder = orderpair.Value.GetTickets();
			break;
		}

		rnd--;
	}

	if (returnorder.Num() <= 0)
		UE_LOG(LogTemp, Warning, TEXT("AAIPlayer, CalculateBuyOrder send out zero orders"))

		return returnorder;
}

void UAIStates::SampleBuildSite()
{
	if (world->GetTimerManager().GetTimerRemaining(stateOwner->buildCooldownHandle) > 0)
	{
		if (stateOwner->validStatesToTick.Contains(EAIStates::AIS_BuildSite))
			stateOwner->validStatesToTick.Remove(EAIStates::AIS_BuildSite);
	}
	else
	{
		if (!stateOwner->validStatesToTick.Contains(EAIStates::AIS_BuildSite))
			stateOwner->validStatesToTick.Add(EAIStates::AIS_BuildSite);
	}
}

TMap<EProductionSiteType, ABuildingSite*> UAIStates::ChooseSiteTypePair()
{
	TMap<EProductionSiteType, ABuildingSite*> chosensitetype;

	return chosensitetype;
}
