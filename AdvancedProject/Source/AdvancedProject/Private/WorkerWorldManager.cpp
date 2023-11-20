// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkerWorldManager.h"

#include "EnumLibrary.h"
#include "Worker.h"
#include "NavigationSystem.h"
#include "Productionsite.h"
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

void FWWM_OverloadFuncs::InitWorkerWorldManager(FWorkerWorldManagerSaveData _saveData, TArray<AProductionsite*> _allProductionSites)
{
	overloadOwner->world = overloadOwner->GetWorld();
	overloadOwner->navigationSystem = Cast<UNavigationSystemV1>(overloadOwner->world->GetNavigationSystem());
	overloadOwner->allProductionSites = _allProductionSites;

	if(!overloadOwner->NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, !NullcheckDependencies()"));
		return;
	}
	
	overloadOwner->SpawnAllWorker(_saveData);
}

void FWWM_OverloadFuncs::InitWorkerWorldManager()
{
	overloadOwner->world = overloadOwner->GetWorld();
	overloadOwner->navigationSystem = Cast<UNavigationSystemV1>(overloadOwner->world->GetNavigationSystem());
	
	if (!overloadOwner->NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, !NullcheckDependencies()"));
		return;
	}
	
	TArray<FWorkerSaveData> allworkersavedata;
	
	for (size_t i = 0; i < overloadOwner->stdWorkerSpawnAmount; i++)
	{
		FNavLocation spawnnavpos;
		FVector spawnpos;
		FRotator spawnrot;
		USkeletalMesh* mesh;
		float rndyaw; 
	
		overloadOwner->navigationSystem->GetRandomPoint(spawnnavpos);
	
		spawnpos = spawnnavpos.Location;
		spawnpos.Z += overloadOwner->capsuleHightValue;
	
		rndyaw = FMath::RandRange(-180, 180);
		spawnrot = FRotator(0, 0, 0);
	
		mesh = overloadOwner->possibleSkeletalMeshes[FMath::RandRange(0, overloadOwner->possibleSkeletalMeshes.Num() - 1)];
	
		FWorkerSaveData workersavedata =
		{
			spawnpos,
			spawnrot,
			mesh,
			overloadOwner->allWorker.Num(),
			EWorkerStatus::WS_Unemployed,
			-1
		};
	
		allworkersavedata.Add(workersavedata);
	}
	
	FWorkerWorldManagerSaveData newsavedata =
	{
		allworkersavedata
	};

	overloadOwner->SpawnAllWorker(newsavedata);
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
		for (size_t i = 0; i < maxSpawnTries; i++)
		{
			FVector spawnpos = workerdata.GetWorkerLocation();
			FRotator spawnrot = workerdata.GetWorkerRotation();
			int siteid = workerdata.GetProductionSiteID();
			EWorkerStatus employementstatus = workerdata.GetEmploymentStatus();

			FActorSpawnParameters params;
			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			
			if (AActor* tospawn = world->SpawnActor(workerClass, &spawnpos, &spawnrot, params))
			{
				AWorker* worker = Cast<AWorker>(tospawn);
				worker->InitWorker(workerdata, navigationSystem, allWorker.Num(), employementstatus, siteid);

				for(AProductionsite* site : allProductionSites)
				{
					if (site->GetLocalProdSiteID() == siteid)
						site->SubscribeWorker(worker);
				}

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
	if(!_toSub)
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, !workerClass"));

	if (!allWorker.Contains(_toSub))
		allWorker.Add(_toSub);

	if (!allUnemploymentWorker.Contains(_toSub))
		allUnemploymentWorker.Add(_toSub);
}

void AWorkerWorldManager::UnsubWorkerFromUnemploymentPool(AWorker* _toUnsub)
{
	if (allUnemploymentWorker.Contains(_toUnsub))
		allUnemploymentWorker.Remove(_toUnsub);
}

void AWorkerWorldManager::SubWorkerToUnemploymentPool(AWorker* _toSub)
{
	if (!allUnemploymentWorker.Contains(_toSub))
		allUnemploymentWorker.Add(_toSub);
}
