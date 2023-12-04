// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeLibrary.h"
#include "Worker.h"
#include "GameFramework/Actor.h"
#include "WorkerWorldManager.generated.h"


class UDataTable;
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

USTRUCT(BlueprintType)
struct FWorkerRefArray
{
	GENERATED_BODY()
		FWorkerRefArray(){}

public:
	FORCEINLINE
		void AddWorker(AWorker* _toAdd) { if (!workerArray.Contains(_toAdd)) workerArray.Add(_toAdd); }
	FORCEINLINE
		void RemoveWorker(AWorker* _toRemove) { if (workerArray.Contains(_toRemove)) workerArray.Remove(_toRemove); }

	FORCEINLINE
		TArray<AWorker*> GetWorkerArray() { return workerArray; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TArray<AWorker*> workerArray;
};




UCLASS()
class ADVANCEDPROJECT_API AWorkerWorldManager : public AActor
{
	GENERATED_BODY()

public:	
	AWorkerWorldManager();

protected:
	// Worker Owner ist die World
	// Alle not hired worker in der welt
	UFUNCTION()
		void SubscribeToWorldPool(AWorker* _toSub);
	UFUNCTION()
		void SubscribeToSideJobPool(AWorker* _toSub);

	// Worker Owner ist immer ein Player
	// Nimmt einen neuen owner an und ersetzt die wolrd als jenen, ab hier wird der owner immer gleich bleiben bis zur entlassung
	// Unassigned == Hired but not assigned to a productionside
	UFUNCTION()
		void SubscribeToUnassignedPool(AWorker* _toSub, EPlayerIdent _newOwner);
	UFUNCTION()
		void SubscribeToAssignedPool(AWorker* _toSub);


protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=WorkerInfo,meta=(AllowPrivateAccess))
		TArray<AWorker*> allWorker_Ref;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		TMap<EPlayerIdent, FWorkerRefArray> allWorker_WorldPool;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		TMap<EPlayerIdent, FWorkerRefArray> allWorker_SideJobPool;

	// All hired but not assigned worker
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		TMap<EPlayerIdent, FWorkerRefArray> allWorker_UnassignedPool;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		TMap<EPlayerIdent, FWorkerRefArray> allWorker_AssignedPool;

public:
	UFUNCTION(BlueprintCallable) FORCEINLINE
		TArray<AWorker*> GetWorker_AllRef() { return allWorker_Ref; }

	UFUNCTION(BlueprintCallable) FORCEINLINE
		TMap<EPlayerIdent, FWorkerRefArray> GetWorker_WorldPool() { return allWorker_WorldPool; }
	UFUNCTION(BlueprintCallable) FORCEINLINE
		TMap<EPlayerIdent, FWorkerRefArray> GetWorker_SideJobPool() { return allWorker_SideJobPool; }


	UFUNCTION(BlueprintCallable) FORCEINLINE
		TMap<EPlayerIdent, FWorkerRefArray> GetWorker_UnassignedPool() { return allWorker_UnassignedPool; }
	UFUNCTION(BlueprintCallable) FORCEINLINE
		TMap<EPlayerIdent, FWorkerRefArray> GetWorker_AssignedPool() { return allWorker_AssignedPool; }

public:	
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
		FWorkerWorldManagerSaveData GetWorkerManagerSaveData();

	// Subscribes worker to the worker world pool -> this is to hold all worker in the world to manage their status and for saving, workers should only be added at Init
	UFUNCTION(BlueprintCallable)
		void UpdateWorkerStatus(AWorker* _toSub, EPlayerIdent _playerIdent);

	UFUNCTION(BlueprintCallable)
		void AddProductionSite(AProductionsite* _toadd);

private:
	UFUNCTION()
		void SpawnAllWorker(FWorkerWorldManagerSaveData _saveData);

	UFUNCTION()
		bool NullcheckDependencies(); 

	UFUNCTION()
		FWorkerDesireBase GenerateWorkerAttribute();

	friend struct FWWM_OverloadFuncs;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorldInfos, meta = (AllowPrivateAccess))
		TArray<class AProductionsite*> allProductionSites;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorldInfos, meta = (AllowPrivateAccess))
		AMarketManager* marketManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		TSubclassOf<AWorker> workerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		TArray<USkeletalMesh*> possibleSkeletalMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		float capsuleHightValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		int maxSpawnTries;


	UPROPERTY()
		UWorld* world;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnInfos, meta = (AllowPrivateAccess))
		int stdWorkerSpawnAmount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnInfos, meta = (AllowPrivateAccess))
		int workerAttributeAmount;

	UPROPERTY()
		UNavigationSystemV1* navigationSystem;
	UPROPERTY()
		class ASaveGameManager* saveGameManager;

	UPROPERTY(EditAnywhere, Category = WorkerDesireTable, meta = (AllowPrivateAccess))
		UDataTable* workerAttributeTable;


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

	void InitWorkerWorldManager(FWorkerWorldManagerSaveData _saveData, TArray<class AProductionsite*> _allProductionSites, ASaveGameManager* _saveGameManager, AMarketManager* _marketManager);
	void InitWorkerWorldManager(ASaveGameManager* _saveGameManager, AMarketManager* _marketManager);
};