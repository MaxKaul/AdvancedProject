// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Productionsite.generated.h"


USTRUCT()
struct FProduktionResources
{
	GENERATED_BODY()

private:
	EResourceIdent resourceIdent;

	int resourceAmount;

	float resourceTickRate;

public:
	FProduktionResources() {  }

	FProduktionResources(EResourceIdent _resourceIdent, int _resourceAmount, float _resourceTickRate)
	{
		resourceIdent = _resourceIdent;
		resourceAmount = _resourceAmount;
		resourceTickRate = _resourceTickRate;
	}

	FORCEINLINE
		EResourceIdent GetResourceIdent() { return resourceIdent; }

	FORCEINLINE
		int GetResourceAmount() { return resourceAmount; }

	FORCEINLINE
		float GetResourceTickRate() { return resourceTickRate; }
};

USTRUCT(BlueprintType)
struct FProductionSiteSaveData
{
	GENERATED_BODY()

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

	FProductionSiteSaveData(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite)
	{
		siteMesh = _siteMesh;
		type = _type;
		buildingSite = _buildingSite;
	}
};


UCLASS()
class ADVANCEDPROJECT_API AProductionsite : public AActor
{
	GENERATED_BODY()
	
public:	
	AProductionsite();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
		void InitProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type,  ABuildingSite* _buildingSite);

	UFUNCTION()
		FProductionSiteSaveData GetProductionSiteSaveData();

	FORCEINLINE
		TArray<FProduktionResources> GetCurrentResources() { return productionResource; };

private:
	UFUNCTION()
		bool NullcheckDependencies();

	UFUNCTION()
		void TickResourceProduction();
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		TArray<class AWorker*> employedWorker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		UDataTable* resourceDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		float currentResourceOutput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SiteInfo, meta = (AllowPrivateAccess))
		TArray<FProduktionResources> productionResource;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ResourceInfo)
		float resourceTickRate;
};
