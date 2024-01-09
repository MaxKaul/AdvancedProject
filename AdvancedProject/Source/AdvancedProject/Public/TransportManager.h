// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarketManager.h"
#include "Components/ActorComponent.h"
#include "TransportManager.generated.h"

USTRUCT(BlueprintType)
struct FTransportOrder
{
	GENERATED_BODY()

	FTransportOrder(){}


	void InitOrder()
	{
		
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hash)
		FString structName_Saved = FString("NONE");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hash)
		FString productionSiteID_Saved = FString("NONE");
public:
	FORCEINLINE
		bool operator==(const  FTransportOrder& _other) const
	{
		return Equals(_other);
	}

	FORCEINLINE
		bool operator!=(const  FTransportOrder& _other) const
	{
		return !Equals(_other);
	}

	FORCEINLINE
		bool Equals(const  FTransportOrder& _other) const
	{
		return structName_Saved == _other.structName_Saved && productionSiteID_Saved == _other.productionSiteID_Saved;
	}


	UPROPERTY()
		TArray<FResourceTransactionTicket> transactionOrder;
};

FORCEINLINE uint32 GetTypeHash(const  FTransportOrder& _this)
{
	const uint32 Hash = FCrc::MemCrc32(&_this, sizeof(FTransportOrder));
	return Hash;
}

USTRUCT()
struct FTransportManagerSaveData
{
	GENERATED_BODY()

	FTransportManagerSaveData() {}

	FTransportManagerSaveData(TMap<FTransportOrder, float> _transactionTravelTimePair)
	{
		s_transactionTravelTimePair = _transactionTravelTimePair;
	}

private:
	UPROPERTY()
		TMap<FTransportOrder, float> s_transactionTravelTimePair;


public:
	FORCEINLINE
		TMap<FTransportOrder, float> GetTransactrionTravelTimePair_S() { return s_transactionTravelTimePair; }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVANCEDPROJECT_API UTransportManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTransportManager();


	UFUNCTION()
		FTransportManagerSaveData GetTransportManagerSaveData();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	UPROPERTY(VisibleAnywhere,Category=info,meta=(AllowPrivateAccess))
		TArray<FResourceTransactionTicket> currentTransaction;

	UPROPERTY()
		class UProductionSiteManager* productionSiteManager;

	UPROPERTY(VisibleAnywhere,meta=(AllowPrivateAccess))
	TMap<FTransportOrder, float> allTransportOrders;

	UPROPERTY()
		UWorld* world;

	UPROPERTY()
		AMarketManager* marketManager;

	friend struct  FTM_OverloadFuncs;
};

// Kann unter umständen auch weg, ich bin mir grad nicht sicher ob ich das auch  machen will
USTRUCT(BlueprintType)
struct FTM_OverloadFuncs
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UTransportManager* overloadOwner;

public:
	FTM_OverloadFuncs(){}

private:
	void InitTransportManager(AMarketManager* _marketManager, UProductionSiteManager* _prodSiteManager);
	void InitTransportManager(FTransportManagerSaveData _saveData, AMarketManager* _marketManager, UProductionSiteManager* _prodSiteManager);

	// Aus game herraus
	void CreateTransportOrder(TArray<FResourceTransactionTicket> _transaction, FVector _startLocation, float _transitSpeed, bool _bGoalIsMarket, class AProductionSite* _originSite, AProductionSite*,TOptional<AProductionSite*> _productionSite);

	// Aus save herraus
	// Erstaml auf hold bis ich den restlichen dreck habe 
	//void CreateTransportOrder(TArray<FResourceTransactionTicket> _transaction, AMarketStall* _goalMarketStall , class AProductionsite* _goalSite, float _remainingTime);
};
