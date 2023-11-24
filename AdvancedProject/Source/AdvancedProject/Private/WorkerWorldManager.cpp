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
			-1,
			EPlayerIdent::PI_DEFAULT
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
		allworkerdata,
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
			EPlayerIdent workerowner = workerdata.GetWorkerOwner();

			FActorSpawnParameters params;
			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			
			if (AActor* tospawn = world->SpawnActor(workerClass, &spawnpos, &spawnrot, params))
			{
				AWorker* worker = Cast<AWorker>(tospawn);
				worker->InitWorker(workerdata, navigationSystem, allWorker.Num(), employementstatus, siteid);

				if(employementstatus == EWorkerStatus::WS_Employed_MainJob)
				{
					for (AProductionsite* site : allProductionSites)
					{
						if (site->GetLocalProdSiteID() == siteid)
							site->SubscribeWorker(worker);
					}

				}

				if (!allWorker.Contains(worker))
					allWorker.Add(worker);

				UpdateWorkerStatus(worker, workerowner);

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

void AWorkerWorldManager::UpdateWorkerStatus(AWorker* _toSub, EPlayerIdent _playerIdent)
{
	if(!_toSub)
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, !workerClass"));

	switch (_toSub->GetEmployementStatus())
	{
	case EWorkerStatus::WS_Unemployed:
		SubWorkerToUnemploymentPool(_toSub, _playerIdent);
		break;
	case EWorkerStatus::WS_Unassigned:
		SubWorkerToUnassignedPool(_toSub, _playerIdent);
		break;
	case EWorkerStatus::WS_Employed_MainJob:
		SubWorkerToMainJobPool(_toSub, _playerIdent);
		break;
	case EWorkerStatus::WS_Employed_SideJob:
		SubWorkerToSideJobPool(_toSub);
		break;

	case EWorkerStatus::WS_DEFAULT:
		SubWorkerToUnemploymentPool(_toSub, _playerIdent);
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, WS_DEFAULT"));
		break;
	case EWorkerStatus::WS_ENTRY_AMOUNT:
		SubWorkerToUnemploymentPool(_toSub, _playerIdent);
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, WS_ENTRY_AMOUNT"));
		break;
	default: ;
	}
}

void AWorkerWorldManager::SubWorkerToUnemploymentPool(AWorker* _toSub, EPlayerIdent _playerIdent)
{
	if (!allWorker_Unemployed.Contains(_toSub))
		allWorker_Unemployed.Add(_toSub);

	UnsubWorkerFromUnassignedPool(_toSub, _playerIdent);
}

void AWorkerWorldManager::UnsubWorkerFromUnemploymentPool(AWorker* _toUnsub)
{
	if (allWorker_Unemployed.Contains(_toUnsub))
		allWorker_Unemployed.Remove(_toUnsub);
}

void AWorkerWorldManager::SubWorkerToUnassignedPool(AWorker* _toSub, EPlayerIdent _playerIdent)
{
	FWorkerRefArray workerref = allWorker_Unassigned.FindRef(_playerIdent);

	if(!workerref.workerArray.Contains(_toSub))
		allWorker_Unassigned.FindRef(_playerIdent).workerArray.Add(_toSub);

	UnsubWorkerToMainJobPool(_toSub, _playerIdent);
	UnsubWorkerToSideJobPool(_toSub);
	UnsubWorkerFromUnemploymentPool(_toSub);
}

void AWorkerWorldManager::UnsubWorkerFromUnassignedPool(AWorker* _toUnsub, EPlayerIdent _playerIdent)
{
	FWorkerRefArray workerref = allWorker_Unassigned.FindRef(_playerIdent);

	if (!workerref.workerArray.Contains(_toUnsub))
		allWorker_Unassigned.FindRef(_playerIdent).workerArray.Add(_toUnsub);
}

void AWorkerWorldManager::SubWorkerToMainJobPool(AWorker* _toSub, EPlayerIdent _playerIdent)
{
	FWorkerRefArray workerref = allWorker_MainJob.FindRef(_playerIdent);

	if (!workerref.workerArray.Contains(_toSub))
		allWorker_MainJob.FindRef(_playerIdent).workerArray.Add(_toSub);

	UnsubWorkerFromUnassignedPool(_toSub, _playerIdent);
}

void AWorkerWorldManager::UnsubWorkerToMainJobPool(AWorker* _toUnsub, EPlayerIdent _playerIdent)
{
	FWorkerRefArray workerref = allWorker_MainJob.FindRef(_playerIdent);

	if (!workerref.workerArray.Contains(_toUnsub))
		allWorker_MainJob.FindRef(_playerIdent).workerArray.Add(_toUnsub);
}

void AWorkerWorldManager::SubWorkerToSideJobPool(AWorker* _toSub)
{
	if (!allWorker_SideJob.Contains(_toSub))
		allWorker_SideJob.Remove(_toSub);
}

void AWorkerWorldManager::UnsubWorkerToSideJobPool(AWorker* _toUnsub)
{
	if (allWorker_SideJob.Contains(_toUnsub))
		allWorker_SideJob.Remove(_toUnsub);
}

void AWorkerWorldManager::AddProductionSite(AProductionsite* _toadd)
{
	if (!allProductionSites.Contains(_toadd))
		allProductionSites.Add(_toadd);
}
