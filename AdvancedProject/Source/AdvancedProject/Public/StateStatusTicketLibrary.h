#pragma once

#include "CoreMinimal.h"
#include "MarketManager.h"
#include "StateStatusTicketLibrary.generated.h"

USTRUCT()
struct FStateStatusTicket_BuyResources
{
	GENERATED_BODY()

		FStateStatusTicket_BuyResources() {}

	FStateStatusTicket_BuyResources(bool _isSampleValid, TArray<FResourceTransactionTicket> _transactionTicket, class AProductionsite* _owningSite)
	{
		transactionTickets = _transactionTicket;
		bIsSampleValid = _isSampleValid;
		owningSite = _owningSite;
	}

private:
	UPROPERTY()
		TArray<FResourceTransactionTicket> transactionTickets;

	UPROPERTY()
		AProductionsite* owningSite;

	UPROPERTY()
		bool bIsSampleValid;

public:
	FORCEINLINE
		TArray<FResourceTransactionTicket> GetTransactionTickets() { return transactionTickets; }
	FORCEINLINE
		AProductionsite* GetOwningSite() { return owningSite; }

	FORCEINLINE
		bool GetValidity() { return bIsSampleValid; }
};


USTRUCT()
struct FStateStatusTicket_SellResources
{
	GENERATED_BODY()

		FStateStatusTicket_SellResources(){}

	FStateStatusTicket_SellResources(bool _isSampleValid, TArray<FResourceTransactionTicket> _transactionTicket, class AProductionsite* _owningSite, bool _isTransportOrder, 
										AProductionsite* _transportSite = nullptr)
	{
		transactionTickets = _transactionTicket;
		bIsSampleValid = _isSampleValid;
		bIsTransportOrder = _isTransportOrder;
		owningSite = _owningSite;

		if (_transportSite != nullptr)
			transportSite = _transportSite;
	}

private:
	UPROPERTY()
		TArray<FResourceTransactionTicket> transactionTickets;

	UPROPERTY()
		AProductionsite* owningSite;

	UPROPERTY()
		AProductionsite* transportSite;

	UPROPERTY()
		bool bIsSampleValid;

	UPROPERTY()
		bool bIsTransportOrder;

public:
	FORCEINLINE
		TArray<FResourceTransactionTicket> GetTransactionTickets() { return transactionTickets; }
	FORCEINLINE
		AProductionsite* GetOwningSite() { return owningSite; }
	FORCEINLINE
		AProductionsite* GetTransportSite() { if (transportSite) return transportSite; else return nullptr; }

	FORCEINLINE
		bool GetValidity() { return bIsSampleValid; }
	FORCEINLINE
		bool GetIsTransportOrder() { return bIsTransportOrder; }
};


USTRUCT()
struct FStateStatusTicket_BuildProdSite
{
	GENERATED_BODY()

	FStateStatusTicket_BuildProdSite(){}

	FStateStatusTicket_BuildProdSite(bool _isSampleValid, TMap<EProductionSiteType, class ABuildingSite*> _typeSitePair)
	{
		typeSitePair = _typeSitePair;
		bIsSampleValid = _isSampleValid;
	}

private:
	UPROPERTY()
		TMap<EProductionSiteType, class ABuildingSite*> typeSitePair;

	UPROPERTY()
	bool bIsSampleValid;

public:
	FORCEINLINE
		TMap<EProductionSiteType, ABuildingSite*> GetTypeSitePair() { return typeSitePair; }

	FORCEINLINE
		bool GetValidity() { return bIsSampleValid; }
};

USTRUCT()
struct FStateStatusTicket_TransportResources
{
	GENERATED_BODY()

	FStateStatusTicket_TransportResources(){}

	FStateStatusTicket_TransportResources(bool _sampleValididty, AProductionsite* _chosenSite, TArray<FResourceTransactionTicket> _transactionTickets, AProductionsite* _owningSite)
	{
		chosenProdSite = _chosenSite;
		owningSite = _owningSite;
		transactionTickets = _transactionTickets;
		bIsSampleValid = _sampleValididty;
	}

private:
	UPROPERTY()
		AProductionsite* chosenProdSite;
	UPROPERTY()
		AProductionsite* owningSite;
	UPROPERTY()
		bool bIsSampleValid;

	UPROPERTY()
		TArray<FResourceTransactionTicket> transactionTickets;

public:
	FORCEINLINE
		AProductionsite* GetChosenSite() { return chosenProdSite; }
	FORCEINLINE
		AProductionsite* GetOwningSite() { return owningSite; }
	FORCEINLINE
		bool GetValidity() { return bIsSampleValid; }
	FORCEINLINE
		TArray<FResourceTransactionTicket> GetTransactionTickets() { return transactionTickets; }
};

USTRUCT()
struct FStateStatusTicket_HireWorker
{
	GENERATED_BODY()

		FStateStatusTicket_HireWorker() {}

	FStateStatusTicket_HireWorker(bool _sampleValididty, AWorker* _workerToHire)
	{
		bIsSampleValid = _sampleValididty;
		workerToHire = _workerToHire;
	}

private:
	UPROPERTY()
		AWorker* workerToHire;

	UPROPERTY()
		bool bIsSampleValid;

public:
	FORCEINLINE
		bool GetValidity() { return bIsSampleValid; }
	FORCEINLINE
		AWorker* GetWorkerToHire() { return workerToHire; }
};
