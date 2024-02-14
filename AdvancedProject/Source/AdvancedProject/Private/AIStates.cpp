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

	for (FResourceTransactionTicket ticket : tickets)
	{
		stateOwner->AddOrDeductCurrency(-ticket.exchangedCapital);
	}

	return status;
}

bool UAIStates::State_SellResources()
{
	bool status = true;

	if (!stateOwner->sampleResult_SellResources.GetValidity())
		return false;

	TArray<FResourceTransactionTicket> tickets = stateOwner->sampleResult_SellResources.GetTransactionTickets();
	AProductionsite* owningsite = stateOwner->sampleResult_SellResources.GetOwningSite();

	if(stateOwner->sampleResult_SellResources.GetIsTransportOrder())
	{
		TMap<EBuildingType, int> goalidx;
		goalidx.Add(EBuildingType::BT_ProductionSite, stateOwner->sampleResult_SellResources.GetTransportSite()->GetLocalProdSiteID());
		stateOwner->transportManager->CreateTransportOrder(tickets, ETransportOrderStatus::TOS_MoveToProdSite, owningsite, ETransportatOrderDirecrtive::TOD_DeliverToSite, goalidx);
	}
	else
	{
		TMap<EBuildingType, int> goalidx;
		goalidx.Add(EBuildingType::BT_MarketStall, 0);
		stateOwner->transportManager->CreateTransportOrder(tickets, ETransportOrderStatus::TOS_MoveToMarket, owningsite, ETransportatOrderDirecrtive::TOD_SellResources, goalidx);
	}


	return status;
}

bool UAIStates::State_TransportResources()
{
	bool status = true;

	if (!stateOwner->sampleResult_TransportResources.GetValidity())
		return false;

	TArray<FResourceTransactionTicket> tickets = stateOwner->sampleResult_TransportResources.GetTransactionTickets();
	AProductionsite* owningsite = stateOwner->sampleResult_TransportResources.GetOwningSite();

	TMap<EBuildingType, int> goalidx;
	goalidx.Add(EBuildingType::BT_ProductionSite , stateOwner->sampleResult_TransportResources.GetChosenSite()->GetLocalProdSiteID());

	stateOwner->transportManager->CreateTransportOrder(tickets, ETransportOrderStatus::TOS_MoveToProdSite, owningsite, ETransportatOrderDirecrtive::TOD_DeliverToSite, goalidx);

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

		TArray<FResourceTransactionTicket> emptytickets;
		stateOwner->sampleResult_BuyResources = FStateStatusTicket_BuyResources(false, emptytickets, nullptr);
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

		TArray<FProductionResources> prodresources = site->GetProductionResources();
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

		TMap<EResourceIdent, float> buypref = stateOwner->currentBehaviourBase.GetBuyPreferences();
		TArray<EResourceIdent> prefidents;
		buypref.GenerateKeyArray(prefidents);

		float prefpropability = 0.f;

		for (TTuple<AProductionsite*, FPossibleBuyOrders> orderpair : possiblesiteorderpairs)
		{
			// Für die buy pref
			for(FResourceTransactionTicket ordertickets : orderpair.Value.GetTickets())
			{
				if (prefidents.Contains(ordertickets.resourceIdent))
					prefpropability += buypref.FindRef(ordertickets.resourceIdent);

				if (prefpropability >= stateOwner->decisionThreshold)
					break;
			}

			if (rnd <= 0 || prefpropability >= stateOwner->decisionThreshold)
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
		if (site->GetHasBeenBuildOn())
			allinvalids++;
	}

	if (allinvalids >= buildingsites.Num())
		bfreesitesavailable = false;

	if (world->GetTimerManager().GetTimerRemaining(stateOwner->buildCooldownHandle) > 0 || buildingsites.Num() <= 0 || !bfreesitesavailable)
	{
		if (stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_BuildSite))
			stateOwner->validStatesToTick.Remove(EPossibleAIStates::PAIS_BuildSite);

		stateOwner->sampleResult_BuildSite = FStateStatusTicket_BuildProdSite(false, ChooseSiteTypePair());
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
// So wie ich das grasde mache sample ich nur die over all availability und nicht die spezifische, ich könnte im vorhinein zwischen den inneren und äußeren build sites sample -> i.e allowed sites
TMap<EProductionSiteType, ABuildingSite*> UAIStates::ChooseSiteTypePair()
{
	TMap<EProductionSiteType, ABuildingSite*> chosensitetypepair;

	TArray<ABuildingSite*> allbuildingsites = stateOwner->allBuildingSites;
	TArray<EProductionSiteType> allprodsitetypes = stateOwner->allProdSiteTypes;
	TMap<EProductionSiteType, float> prefvalues = stateOwner->currentBehaviourBase.GetSitePreferenceValuePair();

	float threshold = stateOwner->decisionThreshold;

	ABuildingSite* chosensite;
	EProductionSiteType chosentype;

	TArray<int32> shufflearray;


	if (allprodsitetypes.Num() > 0)
	{
		int32 LastIndex = allprodsitetypes.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				allprodsitetypes.Swap(i, Index);
			}
		}
	}

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
		if (site->GetAllowedTypes().Contains(chosentype) && !site->GetHasBeenBuildOn())
			possiblesites.Add(site);
	}

	int rnd = FMath::RandRange(0, possiblesites.Num() - 1);

	for(ABuildingSite* site : possiblesites)
	{
		if (rnd <= 0)
		{
			chosensite = site;
			break;
		}

		rnd--;
	}

	if (stateOwner->GetProductionSiteManager()->GetAllProductionSites().Num() <= 0)
		chosentype = EProductionSiteType::PST_Gold_Iron;
	else if (stateOwner->GetProductionSiteManager()->GetAllProductionSites().Num() == 1)
		chosentype = EProductionSiteType::PST_Furniture_Jewelry;

	chosensitetypepair.Add(chosentype, chosensite);

	return chosensitetypepair;
}

void UAIStates::SampleTransportResources()
{
	if(stateOwner->GetProductionSiteManager()->GetAllProductionSites().Num() <= 1)
	{
		if(stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_TransportResources))
			stateOwner->validStatesToTick.Remove(EPossibleAIStates::PAIS_TransportResources);
	}
	else if(stateOwner->GetProductionSiteManager()->GetAllProductionSites().Num() > 1)
	{
		if (!stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_TransportResources))
			stateOwner->validStatesToTick.Add(EPossibleAIStates::PAIS_TransportResources);

		stateOwner->sampleResult_TransportResources = ChooseTransportationStartEndPair();

		if(!stateOwner->sampleResult_TransportResources.GetValidity())
		{
			if (stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_TransportResources))
				stateOwner->validStatesToTick.Remove(EPossibleAIStates::PAIS_TransportResources);
		}
	}
}

// Ist grad nur mit einer resource at a time
FStateStatusTicket_TransportResources UAIStates::ChooseTransportationStartEndPair()
{
	FStateStatusTicket_TransportResources returnticket;

	TArray<AProductionsite*> allsites = stateOwner->GetProductionSiteManager()->GetAllProductionSites();

	TArray<FTransportSample_Deliver> validtodeliverto;

	// Ich schaue welche site alle unter dem resource amount limit in ihren benötigten resourcen sind
	for(AProductionsite* site : allsites)
	{
		TMap<EResourceIdent, int> poolinfo = site->GetProductionSitePoolInfo();
		TArray<FProductionResources> prodresources = site->GetProductionResources();

		for(FProductionResources resource : prodresources)
		{
			if(!resource.GetHasCost())
				continue;

			TArray<EResourceIdent> neededresourcesidents;
			resource.GetResourceCost().GenerateKeyArray(neededresourcesidents);

			TMap<EResourceIdent, int> needetidentamountpair;

			for(EResourceIdent costingresource : neededresourcesidents)
			{
				float resourceamount = poolinfo.FindRef(costingresource);

				if(resourceamount < stateOwner->currentBehaviourBase.GetResourceAmountGoal())
				{
					float amounttosend = stateOwner->currentBehaviourBase.GetResourceAmountGoal() - resourceamount;
					needetidentamountpair.Add(costingresource, amounttosend);
				}

			}

			if(needetidentamountpair.Num() <= 0)
				continue;

			FTransportSample_Deliver newsampel = { site, needetidentamountpair };
			validtodeliverto.Add(newsampel);
		}
	}


	if (validtodeliverto.Num() <= 0)
	{
		TArray<FResourceTransactionTicket> emptytickets;
		return returnticket = FStateStatusTicket_TransportResources{ false, nullptr,  emptytickets, nullptr};
	}

	// In theorie brauch ich heir eign nur ne ref auf meine goaldf prod site, aber ich mach das erstmal so das ich ne ref auf beide samples hab falls ich da nochwas machen musss 
	TMap<FTransportSample_Deliver, FTransportSample_Deliver> validstartgoalpair;

	// Ich schaue welche ob eine meiner site die reosurcen besitzt welche durch die validtodeliverto benötigt werden, danach schaue ich ob diese die benötigen und über dem soll wert liegen
	for(AProductionsite* site : allsites)
	{
		TMap<EResourceIdent, int> poolinfo = site->GetProductionSitePoolInfo();
		float amountgoal = stateOwner->currentBehaviourBase.GetResourceAmountGoal();

		FTransportSample_Deliver takefromsample;

		for (FTransportSample_Deliver deliversample : validtodeliverto)
		{
			if(site == deliversample.GetSite())
				continue;

			TMap<EResourceIdent, int > identamountpair_needed = deliversample.GetResourceIdentAmountPair();
			TMap<EResourceIdent, int> identamountpair_possible;


			for(TTuple<EResourceIdent, int> ident : identamountpair_needed)
			{
				if(poolinfo.FindRef(ident.Key) <= amountgoal)
					continue;

				// ICh nehm erstmal immer das max
				int possibleamount = poolinfo.FindRef(ident.Key) - amountgoal;

				identamountpair_possible.Add(ident.Key, possibleamount);
			}

			FTransportSample_Deliver possiblesample = { site, identamountpair_possible };

			possiblesample.structID = FString::FromInt(site->GetLocalProdSiteID());

			validstartgoalpair.Add(possiblesample, deliversample);
		}
	}


	if (validstartgoalpair.Num() <= 0)
	{
		TArray<FResourceTransactionTicket> emptytickets;
		return returnticket = FStateStatusTicket_TransportResources{ false, nullptr,  emptytickets , nullptr};
	}

	// Ich will die site nehmenb von welcher die KI die meisten benötigten resourcen nehmen kann
	// Ich könnte da unter umständen aucvh nen sort rein machen, brauch aber grad kein zweites sample value
	// Ich weiß, ist kacke, aber ich hab grad kein lentzt ein struct anzulegen (mach ich später vill)


	// Ich will mich immer an der höchsten summe der möglichen werte richten
	// Ich nehme dafür den key welcher immer die possible order sein soll
	// Key == PossibleValue
	FTransportSample_Deliver highestpossiblesample = validstartgoalpair.begin().Key();

	int highestcachesampel = 0;

	for (TTuple<EResourceIdent, int> cachesampel : highestpossiblesample.GetResourceIdentAmountPair())
	{
		highestcachesampel += cachesampel.Value;
	}

	// Ich weiß ein wenig kacke, aber was solls, muss fertig werden
	if(validstartgoalpair.Num() > 1)
	{
		for (TTuple<FTransportSample_Deliver, FTransportSample_Deliver> sample : validstartgoalpair)
		{
			int amountsum = 0;

			for(TTuple<EResourceIdent, int> sampleitem : sample.Key.GetResourceIdentAmountPair())
			{
				amountsum += sampleitem.Value;
			}

			if(amountsum > highestcachesampel)
			{
				highestcachesampel = amountsum;
				highestpossiblesample = sample.Key;
			}
		}
	}

	// Könnt alles scheiße sein und impliodieren weil die hash comparison net klappt

	TArray<FResourceTransactionTicket> transationtickets;

	for (TTuple<EResourceIdent, int> item : highestpossiblesample.GetResourceIdentAmountPair())
	{
		FResourceTransactionTicket newticket = { item.Value, 0, item.Key, 0,0 };

		transationtickets.Add(newticket);
	}

	AProductionsite* transportowner = highestpossiblesample.GetSite();
	AProductionsite* transportgoal = validstartgoalpair.FindRef(highestpossiblesample).GetSite();

	returnticket = { true, transportgoal, transationtickets ,transportowner };

	return returnticket;
}

void UAIStates::SampleSellResources()
{
	if(stateOwner->GetProductionSiteManager()->GetAllProductionSites().Num() <= 0)
	{
		if (stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_SellResources))
			stateOwner->validStatesToTick.Remove(EPossibleAIStates::PAIS_SellResources);

		TArray<FResourceTransactionTicket> emptytickets;
		stateOwner->sampleResult_SellResources = FStateStatusTicket_SellResources(false, emptytickets, nullptr, false);

	}
	else
	{
		if (!stateOwner->validStatesToTick.Contains(EPossibleAIStates::PAIS_SellResources))
			stateOwner->validStatesToTick.Add(EPossibleAIStates::PAIS_SellResources);

		stateOwner->sampleResult_SellResources = CalculateSellOrder();
	}
}

FStateStatusTicket_SellResources UAIStates::CalculateSellOrder()
{
	FStateStatusTicket_SellResources returnticket;

	TArray<AProductionsite*> allsites = stateOwner->GetProductionSiteManager()->GetAllProductionSites();

	TArray<FSellSample_Check> validsiteresourcestotake;

	for(AProductionsite* site : allsites)
	{
		TArray<FProductionResources> productionresource = site->GetProductionResources();
		TMap<EResourceIdent, int> sitepoolinfo = site->GetProductionSitePoolInfo();

		TMap<EResourceIdent, int> valididentamountpairs;

		for(FProductionResources prodresource : productionresource)
		{
			float amount = sitepoolinfo.FindRef(prodresource.GetResourceIdent());

			if (amount >= 150)
				valididentamountpairs.Add(prodresource.GetResourceIdent(), amount);
		}

		if(valididentamountpairs.Num() <= 0)
			continue;

		FSellSample_Check newsample = FSellSample_Check(site, valididentamountpairs);
		validsiteresourcestotake.Add(newsample);
	}

	if (validsiteresourcestotake.Num() <= 0)
		return returnticket = FStateStatusTicket_SellResources(false, TArray<FResourceTransactionTicket>(), nullptr, false);

	TArray<AProductionsite*> siteswithcost;

	for (AProductionsite* checksite : allsites)
	{
		TArray<FProductionResources> prodresources = checksite->GetProductionResources();

		for (FProductionResources resource : prodresources)
		{
			if (!resource.GetHasCost())
				continue;

			if(!siteswithcost.Contains(checksite))
				siteswithcost.Add(checksite);
		}
	}

	TArray<FSellSample_Check> curatedtotake;

	for(FSellSample_Check sample : validsiteresourcestotake)
	{
		TArray<AProductionsite*> transportsites;
		for(AProductionsite* costsites : siteswithcost)
		{
			TArray<FProductionResources> productionresource = costsites->GetProductionResources();
			TMap<EResourceIdent, int> sitepoolinfo = costsites->GetProductionSitePoolInfo();

			TArray<EResourceIdent> valididentcostsonsite;

			for(FProductionResources resource : productionresource )
			{
				for(TTuple<EResourceIdent, int> productioncost : resource.GetResourceCost())
				{
					if (!sample.GetResourceAmountPair().Contains(productioncost.Key))
						continue;

					if(sitepoolinfo.FindRef(productioncost.Key) >= stateOwner->currentBehaviourBase.GetResourceAmountGoal())
						continue;

					valididentcostsonsite.Add(productioncost.Key);
				}
			}

			if(valididentcostsonsite.Num() <= 0)
				continue;

			// Sollte eign nicht vorkommen, weil der output einer site kein inoput sein kann
			if(costsites == sample.GetTakeSite())
				continue;

			transportsites.Add(costsites);
		}

		curatedtotake.Add(FSellSample_Check(sample.GetTakeSite(), sample.GetResourceAmountPair(), transportsites));
	}

	if(curatedtotake.Num() > 0)
		validsiteresourcestotake = curatedtotake;

	// An dieser stelle habe ich mindestens eine site von welcher ich resourcen aus senden kann und geprüft ob ich eine oder mehrere site habe welche die resourcen aus dem output benötigen
	// Ich glaube nicht das ich mehr als einen ident pro site an dieser stelle haben werde, werd ich dann wohl sehen

	// Ich will jetzt schaeun ob eines der Sampel mindestens eine prod site besitzt und dieses dann als valide nehmen
	// Ich schicke an eine random site, nicht die welche es am meisten benötigt -> Falls ich nochmal zu dem projekt zurück komm ist heir mal nen guter ansatzt das die Sites auch gerne etwas mehr tellen können
	// (sparrt mir diesen converluded shit)
	FSellSample_Check validsample = validsiteresourcestotake[0];

	TArray<FSellSample_Check> transportsamples;

	for(FSellSample_Check sample : validsiteresourcestotake)
	{
		if (sample.GetValidTransportSites().Num() <= 0)
			continue;
		
		int rnd = FMath::RandRange(0, sample.GetValidTransportSites().Num() - 1);
		
		sample.SetTransportSite(sample.GetValidTransportSites()[rnd]);

		transportsamples.Add(sample);
	}

	bool bistransportorder = false;;

	if(transportsamples.Num() > 0)
	{
		int rnd = FMath::RandRange(0, transportsamples.Num() - 1);
		validsample = transportsamples[rnd];

		bistransportorder = true;
	}
	
	TArray<FResourceTransactionTicket> tickets;

	for(TTuple<EResourceIdent, int> identamountpair : validsample.GetResourceAmountPair())
	{
		FResourceTransactionTicket currticket = 
		{
			identamountpair.Value,
			0,
			identamountpair.Key,
			0,
			0
		};

		tickets.Add(currticket);
	}

	if (bistransportorder)
		returnticket = FStateStatusTicket_SellResources(true, tickets, validsample.GetTakeSite(), true, validsample.GetTransportSite());
	else
		returnticket = FStateStatusTicket_SellResources(true, tickets, validsample.GetTakeSite(), false);

	return returnticket;
}
