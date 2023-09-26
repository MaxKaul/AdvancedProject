// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "GameFramework/Actor.h"
#include "ProductionSiteManager.generated.h"


USTRUCT(BlueprintType)
struct FProductionSiteManagerSaveData
{
	GENERATED_BODY()

private:
	UPROPERTY()
		TArray<class AProductionsite*> allProductionSites;

public:
	FProductionSiteManagerSaveData() {}

	// MUSS NOCH ERSTEZT WERDEN DURCH DIE INDIVIDDUELLEN SAVE DATEN DER PRODUCTIONSITES
	FProductionSiteManagerSaveData(TArray<AProductionsite*> _allProductionSites)
	{
		allProductionSites = _allProductionSites;
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
	void InitProductionSiteManager();

private:
	UFUNCTION()
		void InitAllProductionSites();
	UFUNCTION()
		bool NullcheckDependencies();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void SubscribeProductionsite( AProductionsite* _newProductionSite);

private:
	UPROPERTY()
	TArray< AProductionsite*> allProductionSites;
	UPROPERTY()
		UWorld* world;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteClasses, meta = (AllowPrivateAccess))
		TSubclassOf<AProductionsite> productionSiteClass;

	//TOptional<AProductionsite> productionsiteSaveData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteClasses, meta = (AllowPrivateAccess))
		TMap<EProductionSiteType, UStaticMesh*> siteKeyMeshPair;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteClasses, meta = (AllowPrivateAccess))
	TArray<class ABuildingSite*> buildingSites;
};

