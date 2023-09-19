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

UCLASS()
class ADVANCEDPROJECT_API AMarketManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AMarketManager();

	bool InitMarketManager(FMarketManagerSaveData _saveData);
	bool InitMarketManager(bool _noSaveData);

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		bool NullcheckDependencies();
	UFUNCTION()
		void InitMarketStalls();
	UFUNCTION()
		void InitResources(FMarketManagerSaveData _saveData);
	UFUNCTION()
		void InitIndividualResource(float _lastResourcePrice, float _priceEvaluationTime,int _resourceAmount);


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
		TArray<FIndividualResourceInfo> resourceList;

	UPROPERTY()
		UWorld* world;
};