// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Worker.h"
#include "GameFramework/Actor.h"
#include "WorkerWorldManager.generated.h"


USTRUCT(BlueprintType)
struct FWorkerWorldManagerSaveData
{
	GENERATED_BODY()

	FWorkerWorldManagerSaveData() {}

	FWorkerWorldManagerSaveData(TArray<FWorkerSaveData> _allWorker)
	{
		allWorker = _allWorker;
	}

public:
	FORCEINLINE
	TArray<FWorkerSaveData> GetAllWorker() { return allWorker; }
	
private:
	UPROPERTY()
		TArray<FWorkerSaveData> allWorker;
	UPROPERTY()
		TArray<FWorkerSaveData> allUnemployedWorker;
};

UCLASS()
class ADVANCEDPROJECT_API AWorkerWorldManager : public AActor
{
	GENERATED_BODY()

public:	
	AWorkerWorldManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
		FWorkerWorldManagerSaveData GetWorkerManagerSaveData();

	// Subscribes worker to the worker world pool -> this is to hold all worker in the world to manage their status and for saving, workers should only be added at Init
	UFUNCTION(BlueprintCallable)
		void UpdateWorkerStatus(AWorker* _toSub);
	// Function to unsub worker from the global Unemployment pool, does not remove them from the World Pool
	// Wird im moment auch noch gecalled wenn ein worker von der productionside auf den productionsidemanager subscribed wird
	UFUNCTION(BlueprintCallable)
		void UnsubWorkerFromUnemploymentPool(AWorker* _toUnsub);
	// Subs the worker to the global Unemployment pool
	UFUNCTION(BlueprintCallable)
		void SubWorkerToUnemploymentPool(AWorker* _toSub);

	UFUNCTION(BlueprintCallable)
		void SubWorkerToUnassignedPool(AWorker* _toSub);
	UFUNCTION(BlueprintCallable)
		void UnsubWorkerFromUnassignedPool(AWorker* _toUnsub);

	UFUNCTION(BlueprintCallable)
		void SubWorkerToMainJobPool(AWorker* _toSub);
	UFUNCTION(BlueprintCallable)
		void UnsubWorkerToMainJobPool(AWorker* _toUnsub);

	UFUNCTION(BlueprintCallable)
		void SubWorkerToSideJobPool(AWorker* _toSub);
	UFUNCTION(BlueprintCallable)
		void UnsubWorkerToSideJobPool(AWorker* _toUnsub);

	UFUNCTION(BlueprintCallable)
		void AddProductionSite(AProductionsite* _toadd);

	UFUNCTION(BlueprintCallable) FORCEINLINE
		TArray<AWorker*> GetAllUnemploymentWorker() { return allWorker_Unemployed; }

private:
	UFUNCTION()
		void SpawnAllWorker(FWorkerWorldManagerSaveData _saveData);

	UFUNCTION()
		bool NullcheckDependencies(); 

	friend struct FWWM_OverloadFuncs;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorldInfos, meta = (AllowPrivateAccess))
		TArray<AWorker*> allWorker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorldInfos, meta = (AllowPrivateAccess))
		TArray<AWorker*> allWorker_Unemployed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorldInfos, meta = (AllowPrivateAccess))
		TArray<AWorker*> allWorker_Unassigned;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorldInfos, meta = (AllowPrivateAccess))
		TArray<AWorker*> allWorker_MainJob;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorldInfos, meta = (AllowPrivateAccess))
		TArray<AWorker*> allWorker_SideJob;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorldInfos, meta = (AllowPrivateAccess))
		TArray<class AProductionsite*> allProductionSites;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		TSubclassOf<AWorker> workerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		TArray<USkeletalMesh*> possibleSkeletalMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		float capsuleHightValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		int maxSpawnTries;
protected:
	UPROPERTY()
		UWorld* world;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnInfos, meta = (AllowPrivateAccess))
		int stdWorkerSpawnAmount;

	UPROPERTY()
	class UNavigationSystemV1* navigationSystem;
};

USTRUCT(BlueprintType)
struct FWWM_OverloadFuncs
{
	GENERATED_BODY()

private:
	UPROPERTY()
	AWorkerWorldManager* overloadOwner;

public:
	FWWM_OverloadFuncs() {}
	FWWM_OverloadFuncs(AWorkerWorldManager* _friend) : overloadOwner(_friend){}

	void InitWorkerWorldManager(FWorkerWorldManagerSaveData _saveData, TArray<class AProductionsite*> _allProductionSites);
	void InitWorkerWorldManager();
};