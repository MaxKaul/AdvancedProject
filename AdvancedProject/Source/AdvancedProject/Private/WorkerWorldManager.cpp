// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkerWorldManager.h"

#include "EnumLibrary.h"
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

	maxSpawnTries = 10;
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

void AWorkerWorldManager::InitWorkerWorldManager(FWorkerWorldManagerSaveData _saveData, TArray<AProductionsite*> _allProductionSites)
{
	world = GetWorld();
	navigationSystem = Cast<UNavigationSystemV1>(world->GetNavigationSystem());
	allProductionSites = _allProductionSites;

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
			mesh,
			allWorker.Num(),
			EWorkerStatus::WS_Unemployed,
			-1
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

// Noch etwas besser machen
void AWorkerWorldManager::SpawnAllWorker(FWorkerWorldManagerSaveData _saveData)
{
	for (FWorkerSaveData workerdata : _saveData.GetAllWorker())
	{
		FVector spawnpos = workerdata.GetWorkerLocation();
		FRotator spawnrot = workerdata.GetWorkerRotation();
		int siteid = workerdata.GetProductionSiteID();
		EWorkerStatus employementstatus = workerdata.GetEmploymentStatus();

		for (size_t i = 0; i < maxSpawnTries; i++)
		{
			FActorSpawnParameters params;
			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			
			if (AActor* tospawn = world->SpawnActor(workerClass, &spawnpos, &spawnrot, params))
			{
				AWorker* worker = Cast<AWorker>(tospawn);
				worker->InitWorker(workerdata, navigationSystem, allWorker.Num(), employementstatus, siteid);
				SubscribeNewWorker(worker);

				break;
			}
		}
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