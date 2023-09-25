// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumLibrary.h"
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
	
	// In seconds with (P(t) = P0 * e(pow(k * t - k * t)))
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float priceEvaluationTime;
};

USTRUCT(BlueprintType)
struct FMarketManagerSaveData
{
	GENERATED_BODY()

	TMap<EResourceIdent, FIndividualResourceInfo> resourceIdentInfoPair;
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

		UPROPERTY()
		FMarketManagerSaveData SAVE_DATA;

public:	
	AMarketManager();

	bool InitMarketManager(FMarketManagerSaveData _saveData);
	bool InitMarketManager(bool _noSaveData);

	FORCEINLINE
		FMarketManagerSaveData GetManagerSaveData() { return SAVE_DATA; }

public:	
	virtual void Tick(float DeltaTime) override;

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

	void InitIndividualResource(float _lastResourcePrice, float _priceEvaluationTime,int _resourceAmount, EResourceIdent _resourceIdent);


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

	UPROPERTY()
		UWorld* world;
};