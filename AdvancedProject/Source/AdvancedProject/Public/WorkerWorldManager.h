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

	FWorkerWorldManagerSaveData(){}

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
		void InitWorkerWorldManager(FWorkerWorldManagerSaveData _saveData); 
		void InitWorkerWorldManager();

	UFUNCTION()
		FWorkerWorldManagerSaveData GetWorkerManagerSaveData();

private:
	UFUNCTION()
		void SpawnAllWorker(FWorkerWorldManagerSaveData _saveData);

	UFUNCTION()
		bool NullcheckDependencies(); 

	UFUNCTION()
		void SubscribeNewWorker(AWorker* _toSub);
	UFUNCTION()
		void UnsubscribeWorker(AWorker* _toUnsub);

private:
	UPROPERTY()
	TArray<AWorker*> allWorker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnInfos, meta = (AllowPrivateAccess))
		TSubclassOf<AWorker> workerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	TArray<USkeletalMesh*> possibleSkeletalMeshes;

	UPROPERTY()
		UWorld* world;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnInfos, meta = (AllowPrivateAccess))
		int stdWorkerSpawnAmount;

	UPROPERTY()
	class UNavigationSystemV1* navigationSystem;
};