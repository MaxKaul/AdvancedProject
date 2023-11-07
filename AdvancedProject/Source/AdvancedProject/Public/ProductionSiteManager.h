// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Productionsite.h"
#include "GameFramework/Actor.h"
#include "ProductionSiteManager.generated.h"

USTRUCT()
struct FProductionSiteManagerOptionals
{
	GENERATED_BODY()

	TOptional<FProductionSiteSaveData> productionsiteSaveData;
};

struct FProductionSiteSaveData;
USTRUCT(BlueprintType)
struct FProductionSiteManagerSaveData
{
	GENERATED_BODY()

private:
	UPROPERTY()
		TArray<FProductionSiteSaveData> productionSiteData;

public:
	FProductionSiteManagerSaveData() {}

	FORCEINLINE
		TArray<FProductionSiteSaveData> GetSaveData() { return productionSiteData; }

	FProductionSiteManagerSaveData(TArray<FProductionSiteSaveData> _productionSiteData)
	{
		productionSiteData = _productionSiteData;
	}

	FORCEINLINE
		void AddProductionSiteSaveData(FProductionSiteSaveData _newSaveData) { productionSiteData.Add(_newSaveData); }
};

// Hällt auch alle arbeiter welche nicht auf productionsides verteilt sind

UCLASS()
class ADVANCEDPROJECT_API UProductionSiteManager : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UProductionSiteManager();

	// Ich gehe im moment davon aus das ich immer savedata habe -> Habe ich keine erstell ich leere
	UFUNCTION()
	void InitProductionSiteManager(class APlayerBase* _managerOwner, FProductionSiteManagerSaveData _saveData, class AMarketManager* _marketManager, class AWorkerWorldManager* _workerWorldManager);

private:
	UFUNCTION()
		void InitAllProductionSites(FProductionSiteManagerSaveData _saveData);


	UFUNCTION()
		bool NullcheckDependencies();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void SubscribeProductionsite(class AProductionsite* _newProductionSite);

	FORCEINLINE
		TArray<AWorker*> GetAllUnasignedWorker() { return subscribedWorker_UnasignedPool; }
	UFUNCTION(BlueprintCallable) FORCEINLINE
		TArray<AWorker*> GetAllHiredWorker(){ return subscribedWorker_HiredPool;};


	UFUNCTION()
		FProductionSiteManagerSaveData GetProductionSiteManagerSaveData();

	FORCEINLINE
		TArray<AProductionsite*> GetAllProductionSites() { return allProductionSites; }

	// Sub und Unsub für worker in den lokalen pool
	// Nicht vergessen -> Die save data wird imemrnoch über den WorkerWorldManager geregelt
	// Function to Subscribe a worker to the local pool i.e employed by a player and Unsubscribes him from the global unemployement pool
	UFUNCTION(BlueprintCallable)
		void SubscribeWorkerToLocalPool(class AWorker* _toSub, bool _subFromSite);
	// Unsubscribe worker from local pool and subscribes him to a productionsite
	UFUNCTION(BlueprintCallable)
		void UnsubscribeWorkerToProductionSite(AWorker* _toUnsub);
	// Unsubscribe worker from local pool and subscribes him to the global unemployement pool
	UFUNCTION(BlueprintCallable)
		void UnsubscribeWorkerToWorldPool(AWorker* _toUnsub);

private:
	UPROPERTY()
		TArray<AProductionsite*> allProductionSites;
	UPROPERTY()
		UWorld* world;

	UPROPERTY()
		APlayerBase* managerOwner;

	UPROPERTY()
		AWorkerWorldManager* workerWorldManager;

	UPROPERTY()
		AMarketManager* marketManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteClasses, meta = (AllowPrivateAccess))
		TSubclassOf<AProductionsite> productionSiteClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PoolInfo, meta = (AllowPrivateAccess))
		TArray<AWorker*> subscribedWorker_UnasignedPool;
	// Alle angestellten worker
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PoolInfo, meta = (AllowPrivateAccess))
		TArray<AWorker*> subscribedWorker_HiredPool;

	UPROPERTY()
		FProductionSiteManagerOptionals productionsiteManagerOptionals;
};

