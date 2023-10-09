// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Productionsite.generated.h"


// Resource Info für die productionsite, FIndividualResourceInfo ist für den Markt
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
	EResourceIdent resourceIdent;

	float resourceAmount;

	// Wir holen uns bei resorucen init die tick rate aus der welt und arbeiten lokal mit diesem wert weiter
	// Dies dient dazu dass die einzelnen productionsites bonis bekommen können welche diesen wert ändert, gillt auch für den resourceMultiplicator
	float resourceTickRate;

public:
	FProductionResources() {  }

	FProductionResources(EResourceIdent _resourceIdent, int _resourceAmount, float _resourceTickRate, const FString& _inMyStruct, const FString& _structID)
	{
		resourceIdent = _resourceIdent;
		resourceAmount = _resourceAmount;
		resourceTickRate = _resourceTickRate;

		structName = _inMyStruct;
		structID = _structID;
	}

	FORCEINLINE
		EResourceIdent GetResourceIdent() { return resourceIdent; }

	FORCEINLINE
		int GetResourceAmount() { return resourceAmount; }

	FORCEINLINE
		float GetResourceTickRate() { return resourceTickRate; }

	FORCEINLINE
		void TickResource(float _amount) { resourceAmount += _amount; }
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
	
public:

	FORCEINLINE
		UStaticMesh* GetSavedMesh(){ return siteMesh; }
	FORCEINLINE
		EProductionSiteType GetSavedType(){ return type; }
	FORCEINLINE
		ABuildingSite* GetSavedBuildingSite(){ return buildingSite; }

	FProductionSiteSaveData() {}

	FProductionSiteSaveData(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite, FString _structName, FString _structID)
	{
		siteMesh = _siteMesh;
		type = _type;
		buildingSite = _buildingSite;
		structName = _structName;
		structID = _structID;
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
	UFUNCTION()
		void InitProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type,  ABuildingSite* _buildingSite, class AMarketManager* _marketManager, int _siteID);

	// Sollte eign alles theoretisch klappen, ist getestet
	UFUNCTION()
		FProductionSiteSaveData GetProductionSiteSaveData();

	UFUNCTION()
		TArray<FProductionResources> GetCurrentResources();

private:
	UFUNCTION()
		bool NullcheckDependencies();

	UFUNCTION()
		void TickResourceProduction(EResourceIdent _ident);
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		TArray<class AWorker*> employedWorker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		UDataTable* resourceDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		float currentResourceOutput;

	// Dies sind die resourcen welche produziert werden
	// Speicher alle lokalen resourcen, also alle welche sich in der produktionsite befinden, von dieser liste aus müssen wir resourcen entnehmen und ticken, enthält auch den timer handle zum ticken der resource
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
		TMap<FProductionResources, FTimerHandle> productionResourceHandlePair;

	// Standard tick value für resourcen, der resourcenoutput pro minute wird  durch die tickrate bestimmt
	// Kann aber auch für bonis und stuff genutzt werden
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ResourceInfo, meta =(AllowPrivateAccess))
		float resourceTickValue;

	UPROPERTY()
		int siteID;
};
