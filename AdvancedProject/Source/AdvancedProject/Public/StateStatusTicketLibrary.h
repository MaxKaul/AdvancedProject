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

	FStateStatusTicket_TransportResources(bool _sampleValididty, AProductionsite* _chosenSite, TArray<FResourceTransactionTicket> _transactionTickets)
	{
		chosenProdSite = _chosenSite;
		transactionTickets = _transactionTickets;
		bIsSampleValid = _sampleValididty;
	}

private:
	UPROPERTY()
		AProductionsite* chosenProdSite;
	UPROPERTY()
		bool bIsSampleValid;

	UPROPERTY()
		TArray<FResourceTransactionTicket> transactionTickets;

public:
	FORCEINLINE
		AProductionsite* GetChosenSite() { return chosenProdSite; }
	FORCEINLINE
		bool GetValidity() { return bIsSampleValid; }
	FORCEINLINE
		TArray<FResourceTransactionTicket> GetTransactionTickets() { return transactionTickets; }
};
