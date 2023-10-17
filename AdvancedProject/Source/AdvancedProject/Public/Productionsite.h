// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Productionsite.generated.h"

// Resource Info für die resourcen welche in der productionsite produziert werden, FIndividualResourceInfo ist für den Markt
// FProductionResources sollte keine infos über die menge der production resource haben es soll nur dazu dienen die infos 
// über die zu produzierende resource zu sichern, falls wir mit save daten arbeiten wird die menge der resorucen über den lokaloen pool geladen
USTRUCT(BlueprintType)
struct FProductionResources
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hash)
		FString structName = FString("NONE");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hash)
		FString structID = FString("NONE");

public:
	FORCEINLINE
	bool operator==(const  FProductionResources& _other) const
	{
		return Equals(_other);
	}

	FORCEINLINE
	bool operator!=(const  FProductionResources& _other) const
	{
		return !Equals(_other);
	}

	FORCEINLINE
	bool Equals(const  FProductionResources& _other) const
	{
		return structName == _other.structName && structID == _other.structID;
	}

private:
	UPROPERTY(VisibleAnywhere, meta =(AllowPrivateAccess))
	EResourceIdent resourceIdent;

	// Wir holen uns bei resorucen init die tick rate aus der welt und arbeiten lokal mit diesem wert weiter
	// Dies dient dazu dass die einzelnen productionsites bonis bekommen können welche diesen wert ändert, gillt auch für den resourceMultiplicator
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		float resourceTickRate;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		bool bHasCost;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TMap<EResourceIdent, int> resourceIdentCostPair;

public:
	FProductionResources() {  }

	FProductionResources(EResourceIdent _resourceIdent, float _resourceTickRate, const FString& _inName, const FString& _inID, bool _bHasCost, TMap<EResourceIdent, int> _resourceIdentCostPair)
	{
		resourceIdent = _resourceIdent;
		resourceTickRate = _resourceTickRate;
		bHasCost = _bHasCost;

		resourceIdentCostPair = _resourceIdentCostPair;

		structName = _inName;
		structID = _inID;
	}

	FORCEINLINE
		EResourceIdent GetResourceIdent() { return resourceIdent; }

	FORCEINLINE
		bool GetHasCost() { return bHasCost; }
	FORCEINLINE
		TMap<EResourceIdent, int> GetResourceCost() { return resourceIdentCostPair; }

	FORCEINLINE
		float GetResourceTickRate() { return resourceTickRate; }
};

FORCEINLINE uint32 GetTypeHash(const  FProductionResources& _this)
{
	const uint32 Hash = FCrc::MemCrc32(&_this, sizeof(FProductionResources));
	return Hash;
}

USTRUCT(BlueprintType)
struct FProductionSiteSaveData
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hash)
		FString structName = FString("NONE");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hash)
		FString structID = FString("NONE");

public:
	FORCEINLINE
		bool operator==(const  FProductionSiteSaveData& _other) const
	{
		return Equals(_other);
	}

	FORCEINLINE
		bool operator!=(const  FProductionSiteSaveData& _other) const
	{
		return !Equals(_other);
	}

	FORCEINLINE
		bool Equals(const  FProductionSiteSaveData& _other) const
	{
		return structName == _other.structName && structID == _other.structID;
	}

private:
	UPROPERTY()
		UStaticMesh* siteMesh;
	UPROPERTY()
		EProductionSiteType type;
	UPROPERTY()
		class ABuildingSite* buildingSite;
	UPROPERTY()
		TMap<EResourceIdent, int> productionSiteResourcePool;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		TMap<FProductionResources, FTimerHandle> productionResourceHandlePair;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		float productionSiteProductivity;

public:

	FORCEINLINE
		UStaticMesh* GetSavedMesh(){ return siteMesh; }
	FORCEINLINE
		EProductionSiteType GetSavedType(){ return type; }
	FORCEINLINE
		ABuildingSite* GetSavedBuildingSite(){ return buildingSite; }
	FORCEINLINE
		TMap<EResourceIdent, int> GetSavedResourcePool() { return productionSiteResourcePool; }
	FORCEINLINE
		TMap<FProductionResources, FTimerHandle> GetSavedResourceHandle() { return productionResourceHandlePair; }



	FProductionSiteSaveData() {}

	FProductionSiteSaveData(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite, FString _structName, FString _structID, 
							TMap<EResourceIdent, int> _productionSiteResourcePool, TMap<FProductionResources, FTimerHandle> _productionResourceHandlePair,
							float _productivity)
	{
		siteMesh = _siteMesh;
		type = _type;
		buildingSite = _buildingSite;
		structName = _structName;
		structID = _structID;
		productionSiteResourcePool = _productionSiteResourcePool;
		productionResourceHandlePair = _productionResourceHandlePair;
		productionSiteProductivity = _productivity;
	}
};

FORCEINLINE uint32 GetTypeHash(const  FProductionSiteSaveData& _this)
{
	const uint32 Hash = FCrc::MemCrc32(&_this, sizeof(FProductionSiteSaveData));
	return Hash;
}

UCLASS()
class ADVANCEDPROJECT_API AProductionsite : public AActor
{
	GENERATED_BODY()
	
public:	
	AProductionsite();

public:	
	virtual void Tick(float DeltaTime) override;

public:
	// Init mit savedata, wird vom ProductionSiteManager aus gecalled
	UFUNCTION()
		void InitProductionSite(FProductionSiteSaveData _saveData, class AMarketManager* marketManager);
	// Init ohne save data, bedeutet die productionsite wurde frisch gebaut
		void InitProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite, class AMarketManager* _marketManager, int _siteID);

	// Sollte eign alles theoretisch klappen, ist getestet
	UFUNCTION()
		FProductionSiteSaveData GetProductionSiteSaveData();

	UFUNCTION()
		TArray<FProductionResources> GetCurrentResources();

private:
	UFUNCTION()
		bool NullcheckDependencies();

	UFUNCTION()
		void TickResourceProduction(EResourceIdent _resourceIdent);
		void TickResourceProduction(EResourceIdent _resourceIdent, TMap<EResourceIdent, int> _resourceCost);

	UFUNCTION()
		void SubscribeWorker(class AWorker* _toSub);
	UFUNCTION()
		void UnsubscribeWorker(AWorker* _toUnsub);

	// Diese function wird nur gecalled wenn eine productionsite frisch gebaut wurde und nicht aus den save daten herraus erstellt wurde
	UFUNCTION()
		void InitResources();

private:
	UPROPERTY()
		bool bWasInit;
	UPROPERTY()
		UStaticMesh* actorMesh;
	UPROPERTY()
		UStaticMeshComponent* actorMeshComponent;

	UPROPERTY()
		ABuildingSite* buildingSite;

	UPROPERTY()
		EProductionSiteType productionSiteType;

	UPROPERTY()
		UWorld* world;

	UPROPERTY()
		 AMarketManager* marketManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		UDataTable* resourceDataTable;

	// Dieser wert ist in abhängigkeit zu der von den subscribedWorker mitgeteilten productivity, welche gelesen wird wenn diese auf die productionsite subscriben
	// Unter umständen kann ich noch eine seperate function zum updaten der productivity machen sollte sich ddiese bei einem der subscribedWorker ändern
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		float productionSiteProductivity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		TArray<AWorker*> subscribedWorker;

	// Dies sind die resourcen welche produziert werden
	// Speichert alle produzierenden resourcen
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
		TMap<FProductionResources, FTimerHandle> productionResourceHandlePair;
	// Speichert alle resourcen welche sich in dieser prodductionsite befinden, wird mit allen resourcen welche sich auf dem markt befinden initialisiert
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		TMap<EResourceIdent, int> productionSiteResourcePool;

	UPROPERTY()
		int siteID;

	// Value by which the resource gets added per tick
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		float resourceStdTickValue;
};
