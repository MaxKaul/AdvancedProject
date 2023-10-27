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


UCLASS()
class ADVANCEDPROJECT_API UProductionSiteManager : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UProductionSiteManager();

	// Ich gehe im moment davon aus das ich immer savedata habe -> Habe ich keine erstell ich leere
	UFUNCTION()
	void InitProductionSiteManager(class APlayerBase* _managerOwner, FProductionSiteManagerSaveData _saveData, class AMarketManager* _marketManager);

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

	UFUNCTION()
		FProductionSiteManagerSaveData GetProductionSiteManagerSaveData();

	FORCEINLINE
		TArray<AProductionsite*> GetAllProductionSites() { return allProductionSites; }

private:
	UPROPERTY()
		TArray<AProductionsite*> allProductionSites;
	UPROPERTY()
		UWorld* world;

	UPROPERTY()
		APlayerBase* managerOwner;

	UPROPERTY()
		AMarketManager* marketManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteClasses, meta = (AllowPrivateAccess))
		TSubclassOf<AProductionsite> productionSiteClass;

	UPROPERTY()
		FProductionSiteManagerOptionals productionsiteManagerOptionals;
};

