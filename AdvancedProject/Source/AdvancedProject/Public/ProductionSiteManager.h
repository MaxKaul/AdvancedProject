// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "GameFramework/Actor.h"
#include "ProductionSiteManager.generated.h"


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
};


UCLASS()
class ADVANCEDPROJECT_API AProductionSiteManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProductionSiteManager();

	UFUNCTION()
	void InitProductionSiteManager(AActor* _managerOwner, FProductionSiteManagerSaveData _saveData, class AMarketManager* _marketManager);
	void InitProductionSiteManager(AActor* _managerOwner, AMarketManager* _marketManager, class ABuildingSite* _TESTBSITE);

private:
	UFUNCTION()
		void InitAllProductionSites(FProductionSiteManagerSaveData _saveData);


	UFUNCTION()
		bool NullcheckDependencies();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void SubscribeProductionsite(class AProductionsite* _newProductionSite);

	UFUNCTION()
		FProductionSiteManagerSaveData GetProductionSiteManagerSaveData();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		class UBuilder* TESTBUILDERCOMP;

private:
	UPROPERTY()
		TArray<AProductionsite*> allProductionSites;
	UPROPERTY()
		UWorld* world;

	UPROPERTY()
		AActor* managerOwner;

	UPROPERTY()
		AMarketManager* marketManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteClasses, meta = (AllowPrivateAccess))
		TSubclassOf<AProductionsite> productionSiteClass;

	TOptional<AProductionsite> productionsiteSaveData;


	 // Comps für Testing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteClasses, meta = (AllowPrivateAccess))
		TArray<UStaticMesh*> TESTMESHES;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteClasses, meta = (AllowPrivateAccess))
		TArray<class ABuildingSite*> TESTSITES;
};

