// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumLibrary.h"
#include "MarketStall.h"
#include "MarketManager.generated.h"


// Resource Info für den Markt, FProduktionResources ist für die individuellen Produktionsites, muss eventuell noch eine bessere lösung her um encapsulation ein wenig zu verbessern
USTRUCT(BlueprintType)
struct FIndividualResourceInfo
{
	GENERATED_BODY()

	FIndividualResourceInfo(EResourceIdent _resourceIdent, EProductionSiteType _allowedProductionSites, int _resourceAmount, float _lastResourcePrice, float _k_Value, float _lastUpdated, bool _bHasCost,
							TMap<EResourceIdent, int> _resourceIdentCostPair, float _desireFulfillmentValue)
	{
		resourceIdent = _resourceIdent;
		allowedProductionSites = _allowedProductionSites;
		resourceAmount = _resourceAmount;
		lastResourcePrice = _lastResourcePrice;
		k_Value = _k_Value;
		lastUpdated = _lastUpdated;
		bHasResourceCost = _bHasCost;
		desireFulfillmentValue = _desireFulfillmentValue;

		resourceIdentCostPair = _resourceIdentCostPair;
	}

	FIndividualResourceInfo(){}

private:
	// Tehe unique ident of the Resource
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		EResourceIdent resourceIdent;

	// The types of Productionsites in which this Resource can be produced
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		EProductionSiteType allowedProductionSites;

	// The initial amount with which this resource will be Initialized at the start of each new game
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		int resourceAmount;

	// The Resource Price with which the Resource will be initialised at the start of each new game
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float lastResourcePrice;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float desireFulfillmentValue;

	// Decay or Growth value of the individual resource, starts with 0 and is representativ for resources entering/leaving the market
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float k_Value;

	// Ich will den resourcen wert jetzt nach jeder transaction updaten
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float lastUpdated;

	// Bool to inidcate wether of not this resource is dependent, or has a cost in form of another resource
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		bool bHasResourceCost;

	// If this resource has a cost, this map holds the resources that need to be supplied and the the amount of cost resource units are needed to produce ONE unit of Prodction Resource
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), meta = (EditCondition = "bHasResourceCost"))
		TMap<EResourceIdent, int> resourceIdentCostPair;

public:
	FORCEINLINE
		float GetLastUpdated() { return lastUpdated; }
	FORCEINLINE
		int GetResourceAmount() { return resourceAmount; }
	FORCEINLINE
		float GetLastResourcePrice() { return lastResourcePrice; }
	FORCEINLINE
		float Get_K_Value() { return k_Value; }
	FORCEINLINE
		float GetFulfillmentValue() { return desireFulfillmentValue; }
	FORCEINLINE
		bool GetHasCost() { return bHasResourceCost; }
	FORCEINLINE
		EResourceIdent GetResourceIdent() { return resourceIdent; }
	FORCEINLINE
		EProductionSiteType GetAllowedProductionSites() { return allowedProductionSites; }
	FORCEINLINE
		TMap<EResourceIdent, int> GetResourceCost() { return resourceIdentCostPair; };

	FORCEINLINE
		void TickLastUpdated() { lastUpdated++ ; }
	FORCEINLINE
		void SetLastUpdated(float _newValue) { lastUpdated = _newValue; }
	FORCEINLINE
		void SetLastResourcePrice(float _newValue) { lastResourcePrice = _newValue; }
	FORCEINLINE
		void AddResourceAmount(int _addValue) { resourceAmount += _addValue; }
	FORCEINLINE
		void SubtractResourceAmount(int _subValue) { resourceAmount -= _subValue; }
	FORCEINLINE
		void SetResourceAmount(float _newValue) { resourceAmount = _newValue; }
	FORCEINLINE
		void Add_K_Value(float _addValue) { k_Value += _addValue; }
	FORCEINLINE
		void Subtract_K_Value(float _subValue) { k_Value -= _subValue; }
	FORCEINLINE
		void Set_K_Value(float _newValue) { k_Value = _newValue; }
	FORCEINLINE
		void SetResourceIdent(EResourceIdent _ident) { resourceIdent = _ident; }
	FORCEINLINE
		void GetFulfillmentValue(float _fulfillmentValue) { desireFulfillmentValue = _fulfillmentValue; }
};

USTRUCT(BlueprintType)
struct FMarketManagerSaveData
{
	GENERATED_BODY()

private:
	UPROPERTY()
		TMap<EResourceIdent, FIndividualResourceInfo> resourceIdentInfoPair;
	UPROPERTY()
		TArray<FMarketStallSaveData> marketStallSaveData;

public:
	FORCEINLINE
		TMap<EResourceIdent, FIndividualResourceInfo> GetResourceInfoPair() { return resourceIdentInfoPair; }
	FORCEINLINE
		TArray<FMarketStallSaveData> GetMarketStallSaveData() { return marketStallSaveData; }

	FMarketManagerSaveData() {}

	FMarketManagerSaveData(TMap<EResourceIdent, FIndividualResourceInfo> _resourceIdentInfoPair, TArray<FMarketStallSaveData> _marketStallSaveData)
	{
		resourceIdentInfoPair = _resourceIdentInfoPair;
		marketStallSaveData = _marketStallSaveData;
	}
};


// Muss unter umständen woanders hin
USTRUCT(BlueprintType)
struct FResourceTransactionTicket
{
	GENERATED_BODY()

	UPROPERTY()
	int resourceAmount;
	// exchangedCapital ist der wert für ALLE EINHEITEN dieser resource (resourceAmount * (P0) = exchangedCapital)
	UPROPERTY()
	float exchangedCapital;
	UPROPERTY()
	EResourceIdent resourceIdent;
	UPROPERTY()
	float maxBuyPricePerResource;
	UPROPERTY()
	float minSellPricePerResource;

	FResourceTransactionTicket(int _resourceAmount, float _exchangedCapital, EResourceIdent _resourceIdent, float _maxBuyPricePricePerResource, float _minSellPricePricePerResource)
	{
		resourceAmount = _resourceAmount;
		exchangedCapital = _exchangedCapital;
		resourceIdent = _resourceIdent;

		maxBuyPricePerResource = _maxBuyPricePricePerResource;
		minSellPricePerResource = _minSellPricePricePerResource;
	}

	FResourceTransactionTicket() {}
};

UCLASS()
class ADVANCEDPROJECT_API AMarketManager : public AActor
{
	GENERATED_BODY()

public:	
	AMarketManager();

	UFUNCTION()
		FMarketManagerSaveData GetMarketManagerSaveData();

	// Wir geben transactiontickets zurück um dem käufer sowohl resourcen als auch überschuss geld zurück zu geben
	UFUNCTION()
		TArray<FResourceTransactionTicket> BuyResources(TArray<FResourceTransactionTicket> _resourcesToBuy);
	UFUNCTION()
		TArray<FResourceTransactionTicket> SellResources(TArray<FResourceTransactionTicket> _resourcesToSell);

	FORCEINLINE
		TMap<EResourceIdent, FIndividualResourceInfo> GetPoolInfo() { return resourceList; }

	UFUNCTION() FORCEINLINE
		TArray<class AMarketStall*> GetAllMarketStalls() { return spawnedStalls; }

	UFUNCTION() FORCEINLINE
		AMarketStall* GetStallByID(int _IDToFind);

private:
	UFUNCTION()
		bool NullcheckDependencies();
	UFUNCTION()
		void InitMarketStalls(TArray<FMarketStallSaveData> _stallSaveData);
	UFUNCTION()
		void InitResources(FMarketManagerSaveData _saveData);

	// To be called on resourceamountchange
	UFUNCTION()
		void  UpdateResourcePrice(FIndividualResourceInfo _resourceToUpdate);

	UFUNCTION()
		void TickResources();

	UFUNCTION()
	void InitIndividualResource(float _lastResourcePrice,int _resourceAmount, EResourceIdent _resourceIdent, EProductionSiteType _allowedProductionSite, float _resourceTickRate,
						        bool _bHasCost, TMap<EResourceIdent, int> _resourceCost, float _desirefulfillmentValue);

	// Mit jedemn run werden die resourcen wwelche bei den stalls gekauft/verkauf werden können neu generiert
	UFUNCTION()
		TArray<FMarketStallSaveData> GenerateStallTypes();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ComponentInfos, meta = (AllowPrivateAccess))
		TArray<FVector> stallPositions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ComponentInfos, meta = (AllowPrivateAccess))
		TArray<UStaticMesh*> stallMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess))
		TArray<class AMarketStall*> spawnedStalls;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess))
		TSubclassOf<AMarketStall> marketStallClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resources, meta = (AllowPrivateAccess))
		TMap<EResourceIdent ,FIndividualResourceInfo> resourceList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resources, meta = (AllowPrivateAccess))
		class UDataTable* resourceDefaultTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MarketInfo, meta = (AllowPrivateAccess))
		float resourceMinValue;

	UPROPERTY()
		UWorld* world;

	friend struct FMM_OverloadFuncs;
};

USTRUCT(BlueprintType)
struct FMM_OverloadFuncs
{
	GENERATED_BODY()

private:
	UPROPERTY()
		AMarketManager* overloadOwner;

public:
	FMM_OverloadFuncs() {}
	FMM_OverloadFuncs(AMarketManager* _friend) : overloadOwner(_friend) {}

	void InitMarketManager(FMarketManagerSaveData _saveData);
	void InitMarketManager();
};