// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "MarketManager.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetStringLibrary.h"
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
		FString structName_Saved = FString("NONE");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hash)
		FString productionSiteID_Saved = FString("NONE");

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
		return structName_Saved == _other.structName_Saved && productionSiteID_Saved == _other.productionSiteID_Saved;
	}

private:
	UPROPERTY()
		UStaticMesh* siteMesh_Saved;
	UPROPERTY()
		EProductionSiteType type_Saved;
	UPROPERTY()
		class ABuildingSite* buildingSite_Saved;
	UPROPERTY()
		TMap<EResourceIdent, int> productionSiteResourcePool_Saved;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		TMap<FProductionResources, FTimerHandle> productionResourceHandlePair_Saved;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		float productionSiteProductivity_Saved;

public:

	FORCEINLINE
		UStaticMesh* GetSavedMesh(){ return siteMesh_Saved; }
	FORCEINLINE
		EProductionSiteType GetSavedType(){ return type_Saved; }
	FORCEINLINE
		ABuildingSite* GetSavedBuildingSite(){ return buildingSite_Saved; }
	FORCEINLINE
		int GetProductionsiteID(){ return  UKismetStringLibrary::Conv_StringToInt(productionSiteID_Saved); }
	FORCEINLINE
		TMap<EResourceIdent, int> GetSavedResourcePool() { return productionSiteResourcePool_Saved; }
	FORCEINLINE
		TMap<FProductionResources, FTimerHandle> GetSavedResourceHandle() { return productionResourceHandlePair_Saved; }



	FProductionSiteSaveData() {}

	FProductionSiteSaveData(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite, FString _structName, FString _productionSiteID,
							TMap<EResourceIdent, int> _productionSiteResourcePool, TMap<FProductionResources, FTimerHandle> _productionResourceHandlePair,
							float _productivity)
	{
		siteMesh_Saved = _siteMesh;
		type_Saved = _type;
		buildingSite_Saved = _buildingSite;
		structName_Saved = _structName;
		productionSiteID_Saved = _productionSiteID;
		productionSiteResourcePool_Saved = _productionSiteResourcePool;
		productionResourceHandlePair_Saved = _productionResourceHandlePair;
		productionSiteProductivity_Saved = _productivity;
	}
};

FORCEINLINE uint32 GetTypeHash(const  FProductionSiteSaveData& _this)
{
	const uint32 Hash = FCrc::MemCrc32(&_this, sizeof(FProductionSiteSaveData));
	return Hash;
}

USTRUCT(BlueprintType)
struct FProductionSiteDisplayInfos
{
	GENERATED_BODY()

		FProductionSiteDisplayInfos() {}


public:
	FORCEINLINE
		void SetWorkerAmount(int _amount) { workerAmount = _amount; }
	FORCEINLINE
		void SetResourceOutput(float _resourceOutput) { resourceOutput = _resourceOutput; }

public:
	//--> Static Infos
	// Type of the Site, used as key
	UPROPERTY(BlueprintReadOnly ,EditAnywhere, meta = (AllowPrivateAccess))
		EProductionSiteType productionSiteType;
	// Resources Produced by the site
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess))
		FName priductionSiteName;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess))
		FName priductionSiteInfo;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess))
		FName resourcesToProduce;
	// Resources needed by the site
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess))
		FName resourcesNeeded;

	// --> Dynamic values
	UPROPERTY(BlueprintReadOnly ,meta = (AllowPrivateAccess))
		int workerAmount;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
		float resourceOutput;
};


UCLASS()
class ADVANCEDPROJECT_API AProductionsite : public AActor
{
	GENERATED_BODY()
	
public:	
	AProductionsite();

protected:
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
		FProductionSiteSaveData GetProductionSiteSaveData();

	UFUNCTION()
		TArray<FProductionResources> GetCurrentResources();

	UFUNCTION(BlueprintCallable)
		void GetDisplayInfo(FProductionSiteDisplayInfos& _displayInfo);

	// Optionals gehen nicht mit UFunctions, wenn ich einen worker random selecten will muss ich den parameter einfach leer lassen
	UFUNCTION(BlueprintCallable)
		void SubscribeWorker(class AWorker* _toSub = nullptr);
	// _fromManager ist dafür da das wenn ein worker vom manager aus unsubbed, der manager bereits weiß das diese worker unsubbed ist und wir den nicht noch callen müssen
	UFUNCTION(BlueprintCallable)
		void UnsubscribeWorker(AWorker* _toUnsub, bool _fromManager);

	UFUNCTION(BlueprintCallable) FORCEINLINE
		int GetLocalProdSiteID() { return siteID; }

private:
	UFUNCTION()
		bool NullcheckDependencies();

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

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Comps, meta=(AllowPrivateAccess))
		UDataTable* productionSideDisplayTable;

	UPROPERTY()
		UWorld* world;

	UPROPERTY()
	class UProductionSiteManager* productionSiteManager;
		
	UPROPERTY()
		 AMarketManager* marketManager;

	friend struct FPS_OverloadFuncs;
	FPS_OverloadFuncs* overloadFuncs;

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

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		class APlayerBase* siteOwner;

	UPROPERTY()
		int siteID;

	// Value by which the resource gets added per tick
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		float resourceStdTickValue;
};

USTRUCT(BlueprintType)
struct FPS_OverloadFuncs
{
	GENERATED_BODY()

private:
	UPROPERTY()
		AProductionsite* overloadOwner;

public:
	FPS_OverloadFuncs() {}
	FPS_OverloadFuncs(AProductionsite* _friend) : overloadOwner(_friend) {}

	// Init mit savedata, wird vom ProductionSiteManager aus gecalled
	void InitProductionSite(FProductionSiteSaveData _saveData, class AMarketManager* _marketManager, class APlayerBase* _siteOwner, FPS_OverloadFuncs* _overloadFuncs, UProductionSiteManager* _productionSiteManager);
	// Init ohne save data, bedeutet die productionsite wurde frisch gebaut
	void InitProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite, class AMarketManager* _marketManager, int _siteID, APlayerBase* _siteOwner, FPS_OverloadFuncs* _overloadFuncs, UProductionSiteManager* _productionSiteManager);

	void TickResourceProduction(EResourceIdent _resourceIdent);
	void TickResourceProduction(EResourceIdent _resourceIdent, TMap<EResourceIdent, int> _resourceCost);
};