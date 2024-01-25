#pragma once

#include "CoreMinimal.h"
#include "MarketManager.h"
#include "StateStatusTicketLibrary.generated.h"

USTRUCT()
struct FStateStatusTicket_BuyResources
{
	GENERATED_BODY()

		FStateStatusTicket_BuyResources() {}

	FStateStatusTicket_BuyResources(bool _isSampleValid, TArray<FResourceTransactionTicket> _transactionTicket)
	{
		transactionTickets = _transactionTicket;
		bIsSampleValid = _isSampleValid;
	}

private:
	UPROPERTY()
		TArray<FResourceTransactionTicket> transactionTickets;

	UPROPERTY()
		bool bIsSampleValid;

public:
	FORCEINLINE
		TArray<FResourceTransactionTicket> GetTransactionTickets() { return transactionTickets; }

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