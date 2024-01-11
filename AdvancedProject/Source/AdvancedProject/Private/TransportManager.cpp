// Fill out your copyright notice in the Description page of Project Settings.


#include "TransportManager.h"
#include "Productionsite.h"
#include "ProductionSiteManager.h"

UTransportManager::UTransportManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	transportSpeed = 10.f;
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


	if (allTransportOrders.Num() <= 0)
		return;
}

void UTransportManager::InitTransportManager(AMarketManager* _marketManager, UProductionSiteManager* _prodSiteManager)
{
	marketManager = _marketManager;
	productionSiteManager = _prodSiteManager;
	world = GetWorld();
}

void UTransportManager::TestOrder()
{
	TArray<FResourceTransactionTicket> tickets;

	// Test order Deliver to prod site
	FResourceTransactionTicket newticket =
	{
		10,
		0,
		EResourceIdent::ERI_Fruit,
		0,
		 0
	};

	tickets.Add(newticket);

	CreateTransportOrder(tickets, productionSiteManager->GetAllProductionSites()[1], ETransportOrderStatus::TOS_MoveToProdSite, productionSiteManager->GetAllProductionSites()[0], ETransportatOrderDirecrtive::TOD_DeliverToSite);
}

void UTransportManager::CreateTransportOrder(TArray<FResourceTransactionTicket> _transaction, AActor* _goalActor, ETransportOrderStatus _orderStatus, AProductionsite* _owningSite, ETransportatOrderDirecrtive _transportDirective)
{
	TArray<FResourceTransactionTicket> sampletransaction = _transaction;

	FTimerHandle handle;

	float traveltime = (_goalActor->GetActorLocation() / _owningSite->GetActorLocation()).Length();
	traveltime *= transportSpeed;

	UE_LOG(LogTemp, Warning, TEXT("Travel Time %f"), traveltime);

	// Bei TOD_SellResources u. TOD_DeliverToSite versuche ich jeweils resourcen aus meine owner production site zu deducten, dabei sample ich ob diese von den gewüsnschen resourrcen in der order auch die gewünschte menge hat
	// Für TOD_FetchFromSite versuche ich resourcen aus einer anderen site zu holen
	// Sollte dem nicht so sein sende ich entweder max. (also alle resourcen des jeweiligen idents) o. 0 falls ich von der resource <= 0 an menge habe
	// Ich muss bei den drei noch differenzieren, weil ich für meine tranport order den owner nicht switche
	// Ist zwar jetzt nicht perfekt da ich auf ProductionSite casten muss, aber die transport directive darf eh nur für sites genutzt werden
	if (_transportDirective == ETransportatOrderDirecrtive::TOD_SellResources || _transportDirective == ETransportatOrderDirecrtive::TOD_DeliverToSite)
		sampletransaction = SampleSitePool(sampletransaction, _owningSite);
	else if (_transportDirective == ETransportatOrderDirecrtive::TOD_FetchFromSite)
		sampletransaction = SampleSitePool(sampletransaction, Cast<AProductionsite>(_goalActor));

	FTransportOrder neworder =
	{
		sampletransaction,
		handle,
		_goalActor,
		_orderStatus,
		_owningSite,
		_transportDirective
	};

	allTransportOrders.Add(neworder);

	auto transactionlambda = [this, neworder]()
	{
		ManageTransaction(neworder);
	};

	FTimerDelegate  currdelegate;
	currdelegate.BindLambda(transactionlambda);

	world->GetTimerManager().SetTimer(handle, currdelegate, traveltime, false);

	_owningSite->RemoveResourcesFromLocalPool(sampletransaction);
}

void UTransportManager::ManageTransaction(FTransportOrder _orderToHandle)
{
	if(!marketManager || !productionSiteManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTransportManager, !marketManager || !productionSiteManager"));
		return;
	}

	if(_orderToHandle.GetTransactionOrder().Num() <= 0)
	{
		// Wenn keine Transaction Order vorliegt und die Direktive TOD_DeliverToSite ist kehrt der tranporter zu der prod site zurück welche ihn owned nachdem er resourcen zu einer weiteren gesendent hat
		// i.e er bekommt kein tatsächliches return ticket
		if(_orderToHandle.GetOrderDirective() == ETransportatOrderDirecrtive::TOD_DeliverToSite)
		{
			UE_LOG(LogTemp, Warning, TEXT("Transporter arrived back at Owning Production Site"));
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("UTransportManager, _orderToHandle.GetTransactionOrder().Num() <= 0"));

		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Order Was Completed"));

	if(_orderToHandle.GetTransportOrderStatus() == ETransportOrderStatus::TOS_MoveToMarket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Order arrrived at Market"));
		HandleMarketTransaction(_orderToHandle);
	}
	else if(_orderToHandle.GetTransportOrderStatus() == ETransportOrderStatus::TOS_MoveToProdSite)
	{
		UE_LOG(LogTemp, Warning, TEXT("Order arrrived at Productionsite"));

		HandleProdSiteTransaction(_orderToHandle);
	}
}

void UTransportManager::HandleMarketTransaction(FTransportOrder _orderToHandle)
{
	TArray<FResourceTransactionTicket> returnticket;
	AActor* newgoal = _orderToHandle.GetOwningProductionSite();
	AProductionsite* orderowner = _orderToHandle.GetOwningProductionSite();

	if(_orderToHandle.GetOrderDirective() == ETransportatOrderDirecrtive::TOD_BuyResources)
		returnticket = marketManager->BuyResources(_orderToHandle.GetTransactionOrder());
	else if(_orderToHandle.GetOrderDirective() == ETransportatOrderDirecrtive::TOD_SellResources)
		returnticket = marketManager->SellResources(_orderToHandle.GetTransactionOrder());

	if(allTransportOrders.Contains(_orderToHandle))
		allTransportOrders.Remove(_orderToHandle);
	else
		UE_LOG(LogTemp,Warning,TEXT("UTransportManager, !allTransportOrders.Contains(_orderToHandle)"))

	CreateTransportOrder(returnticket, newgoal, ETransportOrderStatus::TOS_MoveToProdSite, orderowner, ETransportatOrderDirecrtive::TOD_DeliverToSite);
}

void UTransportManager::HandleProdSiteTransaction(FTransportOrder _orderToHandle)
{
	TArray<FResourceTransactionTicket> returnticket;
	AProductionsite* goalsite = Cast<AProductionsite>(_orderToHandle.GetGoalActor());
	AProductionsite* orderowner = _orderToHandle.GetOwningProductionSite();

	if (_orderToHandle.GetOrderDirective() == ETransportatOrderDirecrtive::TOD_DeliverToSite)
	{
		UE_LOG(LogTemp, Warning, TEXT("Deliver the resources of this transaction order to the goal site and return an empty ticket"));

		goalsite->AddResourcesToLocalPool(_orderToHandle.GetTransactionOrder());

		returnticket =
		{
			
		};
	}
	else if (_orderToHandle.GetOrderDirective() == ETransportatOrderDirecrtive::TOD_FetchFromSite)
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove the resources of this transaction order from the goal site and return the resulting return ticket"));

		returnticket = goalsite->RemoveResourcesFromLocalPool(_orderToHandle.GetTransactionOrder());
	}

	AActor* newgoal = _orderToHandle.GetOwningProductionSite();

	if (allTransportOrders.Contains(_orderToHandle))
		allTransportOrders.Remove(_orderToHandle);
	else
		UE_LOG(LogTemp, Warning, TEXT("UTransportManager, !allTransportOrders.Contains(_orderToHandle)"));


	CreateTransportOrder(returnticket, newgoal, ETransportOrderStatus::TOS_MoveToProdSite, orderowner, ETransportatOrderDirecrtive::TOD_DeliverToSite);
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