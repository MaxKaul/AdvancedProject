// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkerWorldManager.h"
#include "Worker.h"
#include "NavigationSystem.h"
#include "WorkerController.h"

// Sets default values
AWorkerWorldManager::AWorkerWorldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	stdWorkerSpawnAmount = 10;
	capsuleHightValue = 100.f;
}

// Called when the game starts or when spawned
void AWorkerWorldManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorkerWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorkerWorldManager::InitWorkerWorldManager(FWorkerWorldManagerSaveData _saveData)
{
	world = GetWorld();
	navigationSystem = Cast<UNavigationSystemV1>(world->GetNavigationSystem());

	if(!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, !NullcheckDependencies()"));
		return;
	}

	SpawnAllWorker(_saveData);
}

void AWorkerWorldManager::InitWorkerWorldManager()
{
	world = GetWorld();
	navigationSystem = Cast<UNavigationSystemV1>(world->GetNavigationSystem());

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, !NullcheckDependencies()"));
		return;
	}

	TArray<FWorkerSaveData> allworkersavedata;

	for (size_t i = 0; i < stdWorkerSpawnAmount; i++)
	{
		FNavLocation spawnnavpos;
		FVector spawnpos;
		FRotator spawnrot;
		USkeletalMesh* mesh;
		float rndyaw; 

		navigationSystem->GetRandomPoint(spawnnavpos);

		spawnpos = spawnnavpos.Location;
		spawnpos.Z += capsuleHightValue;

		rndyaw = FMath::RandRange(-180, 180);
		spawnrot = FRotator(0, 0, 0);

		mesh = possibleSkeletalMeshes[FMath::RandRange(0, possibleSkeletalMeshes.Num() - 1)];

		FWorkerSaveData workersavedata =
		{
			spawnpos,
			spawnrot,
			mesh
		};

		allworkersavedata.Add(workersavedata);
	}

	FWorkerWorldManagerSaveData newsavedata =
	{
		allworkersavedata
	};

	SpawnAllWorker(newsavedata);
}

FWorkerWorldManagerSaveData AWorkerWorldManager::GetWorkerManagerSaveData()
{
	TArray<FWorkerSaveData> allworkerdata;

	for (AWorker* worker : allWorker)
	{
		allworkerdata.Add(worker->GetWorkerSaveData());
	}

	FWorkerWorldManagerSaveData savedata =
	{
		allworkerdata
	};

	return savedata;
}

void AWorkerWorldManager::SpawnAllWorker(FWorkerWorldManagerSaveData _saveData)
{
	for (FWorkerSaveData workerdata : _saveData.GetAllWorker())
	{
		FVector spawnpos = workerdata.GetWorkerLocation();
		FRotator spawnrot = workerdata.GetWorkerRotation();

		AActor* tospawn = world->SpawnActor(workerClass, &spawnpos, &spawnrot);
		AWorker* worker = Cast<AWorker>(tospawn);

		if (!worker)
			return;

		worker->InitWorker(workerdata);
		
		SubscribeNewWorker(worker);
	}
}

bool AWorkerWorldManager::NullcheckDependencies()
{
	bool status = true;

	if(!workerClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, !workerClass"));
		status = false;
	}

	return status;
}

void AWorkerWorldManager::SubscribeNewWorker(AWorker* _toSub)
{
	if (!allWorker.Contains(_toSub))
		allWorker.Add(_toSub);
}

void AWorkerWorldManager::UnsubscribeWorker(AWorker* _toUnsub)
{
	if (allWorker.Contains(_toUnsub))
		allWorker.Remove(_toUnsub);
}