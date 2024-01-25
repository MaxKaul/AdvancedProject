// Fill out your copyright notice in the Description page of Project Settings.


#include "AIStates.h"

#include "AIPlayer.h"
#include "Builder.h"

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

	if (!stateOwner->sampleResult_BuyResources.GetValidity())
	{
		//UE_LOG(LogTemp, Warning, TEXT("UAIStates, !stateOwner->sampleResult_BuyResources.GetValidity()"));
		return false;
	}

	TArray<FResourceTransactionTicket> tickets = stateOwner->sampleResult_BuyResources.GetTransactionTickets();
	AProductionsite* owningsite = stateOwner->sampleResult_BuyResources.GetOwningSite();

	TMap<EBuildingType, int> goalidx;
	goalidx.Add(EBuildingType::BT_MarketStall, 0);

	stateOwner->transportManager->CreateTransportOrder(tickets, ETransportOrderStatus::TOS_MoveToMarket, owningsite, ETransportatOrderDirecrtive::TOD_BuyResources, goalidx);

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

	if(stateOwner->allBuildingMeshes.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAIStates, stateOwner->allBuildingMeshes.Num() <= 0"));
		return false;
	}

	if(!stateOwner->sampleResult_BuildSite.GetValidity())
	{
		UE_LOG(LogTemp, Warning, TEXT("UAIStates, !stateOwner->sampleResult_BuildSite.GetValidity()"));
		return false;
	}


	UStaticMesh* mesh = stateOwner->allBuildingMeshes[0];

	// Jetzt nicht perfekt gibt mir aber die möglichkeit einige errors an der stelle abzufangen

	TMap<EProductionSiteType, ABuildingSite*> typesitepair = stateOwner->sampleResult_BuildSite.GetTypeSitePair();

	if(typesitepair.Num() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("typesitepair.Num() > 1"));
		return false;
	}

	EProductionSiteType type = typesitepair.begin().Key();
	ABuildingSite* site = typesitepair.begin().Value();


	stateOwner->builder->BuildProductionSite(mesh, type, site, stateOwner->marketManager, stateOwner->productionSiteManager->GetAllProductionSites().Num() + 1);

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
		if (stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_BuyResources))
			stateOwner->validStatesToTick.Remove(EPossibleAIStates::PAIS_BuyResources);
	}
	else
	{
		if (!stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_BuyResources))
			stateOwner->validStatesToTick.Add(EPossibleAIStates::PAIS_BuyResources, stateOwner->stateProbabilityPair.FindRef(EPossibleAIStates::PAIS_BuyResources));

		FStateStatusTicket_BuyResources calculatedOrder = CalculateBuyOrder();

		if (calculatedOrder.GetTransactionTickets().Num() <= 0)
		{
			if (stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_BuyResources))
				stateOwner->validStatesToTick.Remove(EPossibleAIStates::PAIS_BuyResources);

			stateOwner->sampleResult_BuyResources = FStateStatusTicket_BuyResources(false, calculatedOrder.GetTransactionTickets(), calculatedOrder.GetOwningSite());
		}
		else
			stateOwner->sampleResult_BuyResources = FStateStatusTicket_BuyResources(true, calculatedOrder.GetTransactionTickets(), calculatedOrder.GetOwningSite());
	}
}

// Eine BuyAction ist EINE action, bedeutet ich kaufe resourcen für EINE Site aber rechne im vorhinein aus welche möglichen order ich für alle sites haben kann
// Und nehme daruas dann ein zufällige
// -> Ich muss für das kaufen ja auch noch in betracht ziehen das eine prod site unter umständen keine resourcen benötigt, sondern nur worker zum produzieren
FStateStatusTicket_BuyResources UAIStates::CalculateBuyOrder()
{
	TArray<AProductionsite*> allsite = stateOwner->productionSiteManager->GetAllProductionSites();
	TMap<EResourceIdent, FIndividualResourceInfo> marketpoolinfo = stateOwner->marketManager->GetPoolInfo();
	FStateStatusTicket_BuyResources returnticket;

	// Das ist ja grade auch immer nur ein ticket array prod site einrag, es können aber mehr pro site sein, also sollte ich das in ein struct *yay* wrappen
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

			// ich loope durch alle resourcen durch welche ich brauche und adde diese dann für die buy order nachdem ich diese durch die menge an idents geteilt habe,
			// Sollte in theorie dafür sorgen das die buy order gleichmäßig auf alle resourcen verteilt wird
			// Das ist dann im moment noch nicht an die benötigte menge angepasst
			for(TTuple<EResourceIdent, int> identcostpair : resource.GetResourceCost())
			{
				resourcestobuy.Add(identcostpair.Key, (stateOwner->currentBehaviourBase.GetResourceAmountGoal() - resourceamount) / resource.GetResourceCost().Num());
			}
		}

		// Wenn auf dieser site keine resourcen benötigt werden, continue zur nächsten
		if (resourcestobuy.Num() <= 0)
			continue;

		// On a per site thing
		FPossibleBuyOrders possiblebuyorder;

		// Value = Amount
		for (TTuple<EResourceIdent, int> buyresource : resourcestobuy)
		{
			if (marketpoolinfo.Find(buyresource.Key)->GetResourceAmount() <= 0)
				continue;

			float lastprice = marketpoolinfo.Find(buyresource.Key)->GetLastResourcePrice();

			// Hierdurch sollte ich eign nichtmehr checken müssen ob ich genügend money habe im anschluss
			float possibleexpenditure = availablecurrency / expendituredivider;

			possibleexpenditure /= resourcestobuy.Num();

			int amounttobuy = possibleexpenditure / lastprice;
			// Ref kommentar Z. 221-223

			if (amounttobuy <= 0)
				continue;

			if (amounttobuy >= marketpoolinfo.Find(buyresource.Key)->GetResourceAmount())
				amounttobuy = marketpoolinfo.Find(buyresource.Key)->GetResourceAmount();

			if (amounttobuy >= buyresource.Value)
				amounttobuy = buyresource.Value;

			// Jetzt habe ich eign meinen amount to buy u. die menge an currency welche mitgegebn werden muss
			availablecurrency -= possibleexpenditure;

			amounttobuy /= resourcestobuy.Num();

			float exchangeamount = amounttobuy * lastprice;

			FResourceTransactionTicket newticket =
			{
				amounttobuy,
				exchangeamount,
				buyresource.Key,
				99999,
				0
			};

			possiblebuyorder.AddNewTicket(newticket);
		}

		possiblesiteorderpairs.Add(site, possiblebuyorder);
	}

	if (possiblesiteorderpairs.Num() > 0)
	{
		int rnd = FMath::RandRange(0, possiblesiteorderpairs.Num());

		for (TTuple<AProductionsite*, FPossibleBuyOrders> orderpair : possiblesiteorderpairs)
		{
			if (rnd <= 0)
			{
				returnticket =
				{
					true,
					orderpair.Value.GetTickets(),
					orderpair.Key
				};

				break;
			}

			rnd--;
		}
	}
	// else ist dann eh ein leeres ticket

	return returnticket;
}

void UAIStates::SampleBuildSite()
{
	bool bfreesitesavailable = true;
	TArray<ABuildingSite*> buildingsites = stateOwner->allBuildingSites;
	int allinvalids = 0;

	for(ABuildingSite* site : buildingsites)
	{
		if (site->GetBuildStatus())
			allinvalids++;
	}

	if (allinvalids >= buildingsites.Num())
		bfreesitesavailable = false;

	if (world->GetTimerManager().GetTimerRemaining(stateOwner->buildCooldownHandle) > 0 || buildingsites.Num() <= 0 || !bfreesitesavailable)
	{
		if (stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_BuildSite))
			stateOwner->validStatesToTick.Remove(EPossibleAIStates::PAIS_BuildSite);
	}
	else
	{
		if (!stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_BuildSite))
			stateOwner->validStatesToTick.Add(EPossibleAIStates::PAIS_BuildSite, stateOwner->stateProbabilityPair.FindRef(EPossibleAIStates::PAIS_BuildSite));

		TMap<EProductionSiteType, ABuildingSite*> chosentypesitepair = ChooseSiteTypePair();

		if(chosentypesitepair.Num() <= 0)
		{
			if (stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_BuildSite))
				stateOwner->validStatesToTick.Remove(EPossibleAIStates::PAIS_BuildSite);

			stateOwner->sampleResult_BuildSite = FStateStatusTicket_BuildProdSite(false, ChooseSiteTypePair());
		}

		stateOwner->sampleResult_BuildSite = FStateStatusTicket_BuildProdSite(true, ChooseSiteTypePair());
	}
}

// Muss später vill nochmal überarbeitet werden
// So wie ich das grasde mache sample ich nur die over all availability und nicht die spezifische, ich könnte wäre im vorhinein zwischen den inneren und äußeren build sites sample
TMap<EProductionSiteType, ABuildingSite*> UAIStates::ChooseSiteTypePair()
{
	TMap<EProductionSiteType, ABuildingSite*> chosensitetypepair;

	TArray<ABuildingSite*> allbuildingsites = stateOwner->allBuildingSites;
	TArray<EProductionSiteType> allprodsitetypes = stateOwner->allProdSiteTypes;
	TMap<EProductionSiteType, float> prefvalues = stateOwner->currentBehaviourBase.GetSitePreferenceValuePair();


	float threshold = stateOwner->decisionThreshold;

	ABuildingSite* chosensite;
	EProductionSiteType chosentype;


	// Könnte unter umständen AIDS werden
	while(threshold > 0)
	{
		for (EProductionSiteType type : allprodsitetypes)
		{
			float loss = FMath::RandRange(stateOwner->decicionTickRateMin, stateOwner->decicionTickRateMax);

			if (float prefvalue = prefvalues.Contains(type))
				loss += prefvalue;

			threshold -= loss;

			// Ich mach das mal so damit ich immer emit einem value aus der while gehe
			chosentype = type;
		}
	}


	TArray<ABuildingSite*> possiblesites;


	for(ABuildingSite* site : allbuildingsites)
	{
		if (site->GetAllowedTypes().Contains(chosentype))
			possiblesites.Add(site);
	}

	int rnd = FMath::RandRange(0, allbuildingsites.Num());

	for(ABuildingSite* site : allbuildingsites)
	{
		if (rnd <= 0)
		{
			chosensite = site;
			break;
		}

		rnd--;
	}

	chosensitetypepair.Add(chosentype, chosensite);

	return chosensitetypepair;
}
