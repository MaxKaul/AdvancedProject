
// Fill out your copyright notice in the Description page of Project Settings.


#include "TransportManager.h"
#include "Productionsite.h"
#include "ProductionSiteManager.h"
#include "AdvancedProject/AdvancedProjectCharacter.h"

UTransportManager::UTransportManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	transportSpeed = 10.f;
}


// Called every frame
void UTransportManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTransportManager::InitTransportManager(APlayerBase* _owningPlayer, FTransportManagerSaveData _saveData, AMarketManager* _marketManager, UProductionSiteManager* _prodSiteManager)
{
	marketManager = _marketManager;
	productionSiteManager = _prodSiteManager;
	world = GetWorld();
	owningPlayer = _owningPlayer;


	if (_saveData.GetAllTransportOrders_S().Num() <= 0)
		return;

	LoadOrderFromSave(_saveData);
}

void UTransportManager::TestOrder()
{
	TArray<FResourceTransactionTicket> tickets;

	// Test order Deliver to prod site
	FResourceTransactionTicket newticket_1 =
	{
		5,
		250,
		EResourceIdent::ERI_Ambrosia,
		99999,
		 0
	};

	FResourceTransactionTicket newticket_2 =
	{
		10,
		50,
		EResourceIdent::ERI_Fruit,
		99999,
		 0
	};

	//owningPlayer->AddOrDeductCurrency(-newticket_1.exchangedCapital);
	//owningPlayer->AddOrDeductCurrency(-newticket_2.exchangedCapital);

	tickets.Add(newticket_1);
	tickets.Add(newticket_2);

	TMap<EBuildingType, int> testpair_GOAL;

	testpair_GOAL.Add(EBuildingType::BT_MarketStall, 2);

	CreateTransportOrder(tickets, ETransportOrderStatus::TOS_MoveToMarket, productionSiteManager->GetAllProductionSites()[0], ETransportatOrderDirecrtive::TOD_SellResources, testpair_GOAL);
}


FTransportManagerSaveData UTransportManager::GetTransportManagerSaveData()
{
	TArray<FTransportOrder> saveorders;

	for (TTuple<FTransportOrder, FTimerHandle> order : allTransportOrders)
	{
		float newtime = world->GetTimerManager().GetTimerRemaining(order.Value);
	
		FTransportOrder neworder =
		{
			order.Key.GetTransactionOrder(),
			newtime,
			order.Key.GetGoalActorTypeIDPair(),
			order.Key.GetTransportOrderStatus(),
			order.Key.GetOwningProductionSiteID(),
			order.Key.GetOrderDirective(),
			world
		};

		saveorders.Add(neworder);
	}

	FTransportManagerSaveData newsave =
	{
		saveorders
	};

	return newsave;
}

void UTransportManager::LoadOrderFromSave(FTransportManagerSaveData _saveData)
{
	TArray<FTransportOrder> loadedorders = _saveData.GetAllTransportOrders_S();

	for (FTransportOrder order : loadedorders) 
	{
		allTransportOrders.Add(order);
		
		auto transactionlambda = [this, order]()
		{
			ManageTransaction(order);
		};

		FTimerDelegate  currdelegate;
		currdelegate.BindLambda(transactionlambda);

		FTimerHandle newhandle;
		float remainingtime = order.GetTimeRemaining();

		world->GetTimerManager().SetTimer(newhandle, currdelegate, remainingtime, false);
	}
}

void UTransportManager::CreateTransportOrder(TArray<FResourceTransactionTicket> _transaction,
	ETransportOrderStatus _orderStatus, AProductionsite* _owningSite, ETransportatOrderDirecrtive _transportDirective,
	TMap<EBuildingType, int> _goalTypeIDPair, FVector _overridePos)
{
	TArray<FResourceTransactionTicket> sampletransaction = _transaction;

	AActor* goalActor = GetGoalActor(_goalTypeIDPair);

	// Das problem welches ich hier habe ist das ich eine ref auf mein vorheriges ziel bnenötige um die länge zu errechnen
	// goal und owner sind hier ja gleich

	float pathlenght = (goalActor->GetActorLocation() - _owningSite->GetActorLocation()).Length();

	if(_transportDirective == ETransportatOrderDirecrtive::TOD_ReturnToSite)
		pathlenght = (_overridePos - _owningSite->GetActorLocation()).Length();

	float traveltime = pathlenght / transportSpeed;

	UE_LOG(LogTemp, Warning, TEXT("Travel Time %f"), traveltime);

	// Bei TOD_SellResources u. TOD_DeliverToSite versuche ich jeweils resourcen aus meine owner production site zu deducten, dabei sample ich ob diese von den gewüsnschen resourrcen in der order auch die gewünschte menge hat
	// Für TOD_FetchFromSite versuche ich resourcen aus einer anderen site zu holen
	// Sollte dem nicht so sein sende ich entweder max. (also alle resourcen des jeweiligen idents) o. 0 falls ich von der resource <= 0 an menge habe
	// Ich muss bei den drei noch differenzieren, weil ich für meine tranport order den owner nicht switche
	// Ist zwar jetzt nicht perfekt da ich auf ProductionSite casten muss, aber die transport directive darf eh nur für sites genutzt werden
	if (_transportDirective == ETransportatOrderDirecrtive::TOD_SellResources || _transportDirective == ETransportatOrderDirecrtive::TOD_DeliverToSite)
	{
		sampletransaction = SampleSitePool(sampletransaction, _owningSite);

		_owningSite->RemoveResourcesFromLocalPool(sampletransaction);
	}
	else if (_transportDirective == ETransportatOrderDirecrtive::TOD_FetchFromSite)
		sampletransaction = SampleSitePool(sampletransaction, Cast<AProductionsite>(goalActor));

	FTimerHandle newhandle;

	FTransportOrder neworder =
	{
		sampletransaction,
		traveltime,
		_goalTypeIDPair,
		_orderStatus,
		_owningSite->GetLocalProdSiteID(),
		_transportDirective,
		world
	};


	auto transactionlambda = [this, neworder]()
	{
		ManageTransaction(neworder);
	};

	FTimerDelegate  currdelegate;
	currdelegate.BindLambda(transactionlambda);

	world->GetTimerManager().SetTimer(newhandle, currdelegate, traveltime, false);
	allTransportOrders.Add(neworder, newhandle);
}

void UTransportManager::ManageTransaction(FTransportOrder _orderToHandle)
{
	if (!marketManager || !productionSiteManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTransportManager, !marketManager || !productionSiteManager"));
		return;
	}


	if (_orderToHandle.GetTransactionOrder().Num() <= 0)
	{
		// Ich brauch das wohl net mehr weil ich die seperate direktive ETransportatOrderDirecrtive::TOD_ReturnToSite habe
		// Wenn keine Transaction Order vorliegt und die Direktive TOD_DeliverToSite ist kehrt der tranporter zu der prod site zurück welche ihn owned nachdem er resourcen zu einer weiteren gesendent hat
		// i.e er bekommt kein tatsächliches return ticket
		//if(_orderToHandle.GetOrderDirective() == ETransportatOrderDirecrtive::TOD_DeliverToSite)
		//{

		UE_LOG(LogTemp, Warning, TEXT("Transporter arrived empty at Goal"));

		//	if (allTransportOrders.Contains(_orderToHandle))
		//		allTransportOrders.Remove(_orderToHandle);
		//}
		//else


		// Ich hab den log jetz tmal rausgenommen da dieser case auch eintreten kann wenn ich von einert prodsite to prodsite deliverie zurückl komme
		//UE_LOG(LogTemp, Warning, TEXT("UTransportManager, _orderToHandle.GetTransactionOrder().Num() <= 0"));

		if (allTransportOrders.Contains(_orderToHandle))
			allTransportOrders.Remove(_orderToHandle);

		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Order Was Completed"));


	if (_orderToHandle.GetTransportOrderStatus() == ETransportOrderStatus::TOS_MoveToMarket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Order arrrived at Market"));
		HandleMarketTransaction(_orderToHandle);
	}
	else if (_orderToHandle.GetTransportOrderStatus() == ETransportOrderStatus::TOS_MoveToProdSite)
	{
		UE_LOG(LogTemp, Warning, TEXT("Order arrrived at Productionsite"));

		HandleProdSiteTransaction(_orderToHandle);
	}
}

void UTransportManager::HandleMarketTransaction(FTransportOrder _orderToHandle)
{
	TArray<FResourceTransactionTicket> returnticket;

	AProductionsite* orderowner = productionSiteManager->GetSiteByID(_orderToHandle.GetOwningProductionSiteID());

	if(_orderToHandle.GetOrderDirective() == ETransportatOrderDirecrtive::TOD_BuyResources)
		returnticket = marketManager->BuyResources(_orderToHandle.GetTransactionOrder());
	else if(_orderToHandle.GetOrderDirective() == ETransportatOrderDirecrtive::TOD_SellResources)
		returnticket = marketManager->SellResources(_orderToHandle.GetTransactionOrder());

	// Ich will das wenn eine markt transaction ausgeführt wird auch die currency erst addiert wird wenn der transporter die delivery gemacht hat, muss dann aber drauf achten das ich das auch auf den player machee

	if (allTransportOrders.Contains(_orderToHandle))
		allTransportOrders.Remove(_orderToHandle);
	else
		UE_LOG(LogTemp, Warning, TEXT("UTransportManager, !allTransportOrders.Contains(_orderToHandle)"));

	if(_orderToHandle.GetOrderDirective() != ETransportatOrderDirecrtive::TOD_ReturnToSite)
	{
		TMap<EBuildingType, int> newgoaltypeid;
		newgoaltypeid.Add(EBuildingType::BT_ProductionSite, _orderToHandle.GetOwningProductionSiteID());

		FVector oldgoal = GetGoalActor(_orderToHandle.GetGoalActorTypeIDPair())->GetActorLocation();

		CreateTransportOrder(returnticket, ETransportOrderStatus::TOS_MoveToProdSite, orderowner, ETransportatOrderDirecrtive::TOD_ReturnToSite, newgoaltypeid, oldgoal);
	}
}

void UTransportManager::HandleProdSiteTransaction(FTransportOrder _orderToHandle)
{
	TArray<FResourceTransactionTicket> returnticket;

	AProductionsite* goalsite = Cast<AProductionsite>(GetGoalActor(_orderToHandle.GetGoalActorTypeIDPair()));
	AProductionsite* orderowner = productionSiteManager->GetSiteByID(_orderToHandle.GetOwningProductionSiteID());

	if (_orderToHandle.GetOrderDirective() == ETransportatOrderDirecrtive::TOD_DeliverToSite || _orderToHandle.GetOrderDirective() == ETransportatOrderDirecrtive::TOD_ReturnToSite)
	{
		UE_LOG(LogTemp, Warning, TEXT("Deliver the resources of this transaction order to goal or owner site and return an empty ticket"));

		goalsite->AddResourcesToLocalPool(_orderToHandle.GetTransactionOrder());

		for(FResourceTransactionTicket ticket : _orderToHandle.GetTransactionOrder())
		{
			owningPlayer->AddOrDeductCurrency(ticket.exchangedCapital);
		}

		returnticket =
		{
			
		};
	}
	else if (_orderToHandle.GetOrderDirective() == ETransportatOrderDirecrtive::TOD_FetchFromSite)
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove the resources of this transaction order from the goal site and return the resulting return ticket"));

		returnticket = goalsite->RemoveResourcesFromLocalPool(_orderToHandle.GetTransactionOrder());
	}

	if (allTransportOrders.Contains(_orderToHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove handlewd order"));
		allTransportOrders.Remove(_orderToHandle);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("UTransportManager, !allTransportOrders.Contains(_orderToHandle)"));

	if(_orderToHandle.GetOrderDirective() != ETransportatOrderDirecrtive::TOD_ReturnToSite)
	{
		TMap<EBuildingType, int> newgoaltypeid;
		newgoaltypeid.Add(EBuildingType::BT_ProductionSite, _orderToHandle.GetOwningProductionSiteID());

		FVector oldgoal = GetGoalActor(_orderToHandle.GetGoalActorTypeIDPair())->GetActorLocation();

		CreateTransportOrder(returnticket, ETransportOrderStatus::TOS_MoveToProdSite, orderowner, ETransportatOrderDirecrtive::TOD_ReturnToSite, newgoaltypeid, oldgoal);
	}
}

AActor* UTransportManager::GetGoalActor(TMap<EBuildingType, int> _goalTypeIDPair)
{
	AActor* goalactor = productionSiteManager->GetSiteByID(0);

	for(auto typeidxpair : _goalTypeIDPair)
	{
		if (typeidxpair.Key == EBuildingType::BT_MarketStall)
		{
			goalactor = marketManager->GetStallByID(typeidxpair.Value);
			break;
		}
		else if (typeidxpair.Key == EBuildingType::BT_ProductionSite)
		{
			goalactor = productionSiteManager->GetSiteByID(typeidxpair.Value);
			break;
		}
	}

	return goalactor;
}

TArray<FResourceTransactionTicket> UTransportManager::SampleSitePool(TArray<FResourceTransactionTicket> _ticketsToCheck, AProductionsite* _siteToSample)
{
	TArray<FResourceTransactionTicket> sampledtickets;

	for(FResourceTransactionTicket ticket : _ticketsToCheck)
	{
		int newamount = ticket.resourceAmount;
		
		for(TTuple<EResourceIdent, int> sitepoolitem : _siteToSample->GetProductionSitePoolInfo())
		{
			if(ticket.resourceIdent == sitepoolitem.Key)
			{
				if(sitepoolitem.Value <= 0)
				{
					newamount = 0;
					break;
				}
				else if(ticket.resourceAmount > sitepoolitem.Value)
				{
					newamount = sitepoolitem.Value;
					break;
				}
			}
		}

		FResourceTransactionTicket newticket = { newamount, ticket.exchangedCapital, ticket.resourceIdent, ticket.marketManagerOptionals.maxBuyPricePerResource, ticket.marketManagerOptionals.minSellPricePerResource};

		sampledtickets.Add(newticket);
	}

	return  sampledtickets;
}
