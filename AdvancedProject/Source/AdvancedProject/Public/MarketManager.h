// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumLibrary.h"
#include "Components/TimelineComponent.h"
#include "MarketManager.generated.h"




USTRUCT(BlueprintType)
struct FIndividualResourceInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		EResourceIdent resourceIdent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		int resourceAmount;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float lastResourcePrice;


	// Decay or Growth value of the individual resource, starts with 0 and is representativ for resources entering/leaving the market
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float k_Value;
};

USTRUCT(BlueprintType)
struct FMarketManagerSaveData
{
	GENERATED_BODY()

private:
	TMap<EResourceIdent, FIndividualResourceInfo> resourceIdentInfoPair;

public:
	FORCEINLINE
		TMap<EResourceIdent, FIndividualResourceInfo> GetResourceInfoPair() { return resourceIdentInfoPair; }

	FMarketManagerSaveData() {}

	FMarketManagerSaveData(TMap<EResourceIdent, FIndividualResourceInfo> _resourceIdentInfoPair)
	{
		resourceIdentInfoPair = _resourceIdentInfoPair;
	}
};


// Muss unter umständen woanders hin
USTRUCT(BlueprintType)
struct FResourceTransactionTicket
{
	GENERATED_BODY()

	int resourceAmount;
	// exchangedCapital ist der wert für ALLE EINHEITEN dieser resource (resourceAmount * (P0) = exchangedCapital)
	float exchangedCapital;
	EResourceIdent resourceIdent;

	TOptional<float> maxBuyPricePerResource;
	TOptional<float> minSellPricePricePerResource;

	FResourceTransactionTicket(int _resourceAmount, float _exchangedCapital, EResourceIdent _resourceIdent, TOptional<float> _maxBuyPricePricePerResource, TOptional<float> _minSellPricePricePerResource)
	{
		resourceAmount = _resourceAmount;
		exchangedCapital = _exchangedCapital;
		resourceIdent = _resourceIdent;

		maxBuyPricePerResource = _maxBuyPricePricePerResource;
		minSellPricePricePerResource = _minSellPricePricePerResource;
	}

	FResourceTransactionTicket() {}
};

UCLASS()
class ADVANCEDPROJECT_API AMarketManager : public AActor
{
	GENERATED_BODY()

public:	
	AMarketManager();

	bool InitMarketManager(FMarketManagerSaveData _saveData);
	bool InitMarketManager();

	UFUNCTION()
		FMarketManagerSaveData GetMarketManagerSaveData();
public:	
	virtual void Tick(float DeltaTime) override;

	int test;


	// Wir geben transactiontickets zurück um dem käufer sowohl resourcen als auch überschuss geld zurück zu geben
	UFUNCTION()
		TArray<FResourceTransactionTicket> BuyResources(TArray<FResourceTransactionTicket> _resourcesToBuy);
	UFUNCTION()
		TArray<FResourceTransactionTicket> SellResources(TArray<FResourceTransactionTicket> _resourcesToSell);

	FORCEINLINE
		TMap<EResourceIdent,FIndividualResourceInfo> GetPoolInfo() { return resourceList; }

private:
	UFUNCTION()
		bool NullcheckDependencies();
	UFUNCTION()
		void InitMarketStalls();
	UFUNCTION()
		void InitResources(FMarketManagerSaveData _saveData);

	UFUNCTION()
		void  UpdateResourcePrices();

	void InitIndividualResource(float _lastResourcePrice,int _resourceAmount, EResourceIdent _resourceIdent);

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
		float resourcePriceTick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MarketInfo, meta = (AllowPrivateAccess))
		float resourceMinValue;

	UPROPERTY()
		UWorld* world;
};