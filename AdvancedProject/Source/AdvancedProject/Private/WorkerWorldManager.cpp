// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkerWorldManager.h"

#include "EnumLibrary.h"
#include "Worker.h"
#include "NavigationSystem.h"
#include "PlayerBase.h"
#include "Productionsite.h"
#include "SaveGameManager.h"
#include "WorkerController.h"

// Sets default values
AWorkerWorldManager::AWorkerWorldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	stdWorkerSpawnAmount = 10;
	capsuleHightValue = 100.f;

	workerStartCurrency = 150.f;

	maxSpawnTries = 10;
	workerDesireDefaultMax = 100.f;
	workerAttributeAmount = 3;
}

// Called every frame
void AWorkerWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void FWWM_OverloadFuncs::InitWorkerWorldManager(FWorkerWorldManagerSaveData _saveData, TArray<AProductionsite*> _allProductionSites, ASaveGameManager* _saveGameManager, AMarketManager* _marketManager)
{
	overloadOwner->world = overloadOwner->GetWorld();
	overloadOwner->navigationSystem = Cast<UNavigationSystemV1>(overloadOwner->world->GetNavigationSystem());
	overloadOwner->allProductionSites = _allProductionSites;
	overloadOwner->saveGameManager = _saveGameManager;
	overloadOwner->marketManager = _marketManager;

	overloadOwner->allWorker_WorldPool.Add(EPlayerIdent::PI_World, FWorkerRefArray());

	if(!overloadOwner->NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, !NullcheckDependencies()"));
		return;
	}
	
	overloadOwner->SpawnAllWorker(_saveData);
}

void FWWM_OverloadFuncs::InitWorkerWorldManager(ASaveGameManager* _saveGameManager, AMarketManager* _marketManager)
{
	overloadOwner->world = overloadOwner->GetWorld();
	overloadOwner->navigationSystem = Cast<UNavigationSystemV1>(overloadOwner->world->GetNavigationSystem());
	overloadOwner->saveGameManager = _saveGameManager;
	overloadOwner->marketManager = _marketManager;

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

		TArray<FWorkerDesireBase> rnddesirebase;

		for (size_t j = 0; j < overloadOwner->workerAttributeAmount; j++)
		{
			rnddesirebase.Add(overloadOwner->GenerateWorkerAttribute());
		}

		// Ich Init die pseudo save data mit default resourceidents damit die preffered buy order aus dem worker init herraus force generated wird
		FWorkerSaveData workersavedata =
		{
			spawnpos,
			spawnrot,
			mesh,
			overloadOwner->allWorker_Ref.Num(),
			EWorkerStatus::WS_Unemployed,
			EWorkerStatus::WS_Unemployed,
			EPlayerIdent::PI_DEFAULT,
			rnddesirebase,
			EResourceIdent::ERI_DEFAULT,
			EResourceIdent::ERI_DEFAULT,
			overloadOwner->workerDesireDefaultMax,
			overloadOwner->workerDesireDefaultMax,
			overloadOwner->workerDesireDefaultMax,
			TArray<int>(),
			0,
			0,
			overloadOwner->workerStartCurrency
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

	for (AWorker* worker : allWorker_Ref)
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
	// Adding the key to the individual lists
	allWorker_WorldPool.Add(EPlayerIdent::PI_World, FWorkerRefArray());
	allWorker_SideJobPool.Add(EPlayerIdent::PI_World, FWorkerRefArray());

	for (size_t i = 0; i < saveGameManager->GetMaxPlayerAmount(); i++)
	{
		allWorker_AssignedPool.Add(EPlayerIdent(i + 1), FWorkerRefArray());
		allWorker_UnassignedPool.Add(EPlayerIdent(i + 1), FWorkerRefArray());
	}

	for (FWorkerSaveData workerdata : _saveData.GetAllWorker())
	{
		for (size_t i = 0; i < maxSpawnTries; i++)
		{
			FVector spawnpos = workerdata.GetWorkerLocation_S();
			FRotator spawnrot = workerdata.GetWorkerRotation_S();
			EWorkerStatus employementstatus = workerdata.GetEmploymentStatus_S();
			EWorkerStatus cachedstatus = workerdata.GetCachedEmploymentStatus_S();
			EPlayerIdent workerowner = workerdata.GetWorkerOwner_S();
			TArray<FWorkerDesireBase> workerdesirebase = workerdata.GetDesireBase_S();

			int siteid = 0;

			if(workerdata.GetWorkerOptionals_S().workProductionSiteID.IsSet())
				siteid = workerdata.GetWorkerOptionals_S().workProductionSiteID.GetValue();

			FActorSpawnParameters params;
			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			
			if (AActor* tospawn = world->SpawnActor(workerClass, &spawnpos, &spawnrot, params))
			{
				AWorker* worker = Cast<AWorker>(tospawn);
				worker->InitWorker(workerdata, navigationSystem, allWorker_Ref.Num(), employementstatus, cachedstatus, workerowner, workerdesirebase, marketManager, workerdata.GetPossibleStallIDs_S());

				if(employementstatus == EWorkerStatus::WS_Assigned_MainJob)
				{
					for (AProductionsite* site : allProductionSites)
					{
						if (site->GetLocalProdSiteID() == siteid)
							site->SubscribeWorker(worker);
					}

				}

				if (!allWorker_Ref.Contains(worker))
					allWorker_Ref.Add(worker);

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

	else if(!workerAttributeTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, !workerDesireTable"));
		status = false;
	}

	return status;
}

FWorkerDesireBase AWorkerWorldManager::GenerateWorkerAttribute()
{
	TArray<FAttributeEffetcTableRow*> tablerowref;

	for(TTuple<FName, unsigned char*> item : workerAttributeTable->GetRowMap())
	{
		tablerowref.Add(reinterpret_cast<FAttributeEffetcTableRow*>(item.Value));
	}

	int rndidx = FMath::RandRange(1, tablerowref[0]->attributeBase.Num());
	EWorkerAttributeIdentifier rndident = (EWorkerAttributeIdentifier)rndidx;
	
	FWorkerDesireBase* base = tablerowref[0]->attributeBase.Find(rndident);

	return *base;
}


void AWorkerWorldManager::UpdateWorkerStatus(AWorker* _toSub, EPlayerIdent _newOwner)
{
	if(!_toSub)
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, !workerClass"));

	switch (_toSub->GetEmployementStatus())
	{
	case EWorkerStatus::WS_Unemployed:
		SubscribeToWorldPool(_toSub);
		break;

	case EWorkerStatus::WS_Assigned_SideJob:
		SubscribeToSideJobPool(_toSub);
		break;

	case EWorkerStatus::WS_Unassigned:
		SubscribeToUnassignedPool(_toSub, _newOwner);
		break;

	case EWorkerStatus::WS_Assigned_MainJob:
		SubscribeToAssignedPool(_toSub);
		break;

	case EWorkerStatus::WS_DEFAULT:
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, UpdateWorkerStatus, Default -> Ignore if Init "))
		break;

	default:
		UE_LOG(LogTemp,Error,TEXT("AWorkerWorldManager, UpdateWorkerStatus "))
		;
	}
}

void AWorkerWorldManager::SubscribeToWorldPool(AWorker* _toSub)
{
	allWorker_WorldPool.Find(EPlayerIdent::PI_World)->AddWorker(_toSub);

	EPlayerIdent previousowner = _toSub->GetCurrentOwner();

	// With no previous owner the worker is most likely subbed without save file
	if (previousowner == EPlayerIdent::PI_DEFAULT || previousowner == EPlayerIdent::PI_World)
		_toSub->SetWorkerOwner(EPlayerIdent::PI_World);
	else
	{
		if (allWorker_SideJobPool.Contains(previousowner))
			allWorker_SideJobPool.Find(previousowner)->RemoveWorker(_toSub);
		else if(allWorker_UnassignedPool.Contains(previousowner))
			allWorker_UnassignedPool.Find(previousowner)->RemoveWorker(_toSub);
		else if (allWorker_AssignedPool.Contains(previousowner))
			allWorker_AssignedPool.Find(previousowner)->RemoveWorker(_toSub);
	}
}

void AWorkerWorldManager::SubscribeToSideJobPool(AWorker* _toSub)
{
	allWorker_SideJobPool.Find(EPlayerIdent::PI_World)->AddWorker(_toSub);

	EPlayerIdent previousowner = _toSub->GetCurrentOwner();

	if (previousowner == EPlayerIdent::PI_DEFAULT)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, SubscribeToSideJobPool, Owner was default"));
		return;
	}

	if (allWorker_WorldPool.Contains(previousowner))
		allWorker_WorldPool.Find(previousowner)->RemoveWorker(_toSub);
	else if (allWorker_UnassignedPool.Contains(previousowner))
		allWorker_UnassignedPool.Find(previousowner)->RemoveWorker(_toSub);
	else if (allWorker_AssignedPool.Contains(previousowner))
		allWorker_AssignedPool.Find(previousowner)->RemoveWorker(_toSub);
}

void AWorkerWorldManager::SubscribeToUnassignedPool(AWorker* _toSub, EPlayerIdent _newOwner)
{
	allWorker_UnassignedPool.Find(_newOwner)->AddWorker(_toSub);

	EPlayerIdent previousowner = _toSub->GetCurrentOwner();

	if (previousowner == EPlayerIdent::PI_DEFAULT)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, SubscribeToUnassignedPool, Owner was default"));
		return;
	}

	_toSub->SetWorkerOwner(_newOwner);

	if (allWorker_WorldPool.Contains(previousowner))
		allWorker_WorldPool.Find(previousowner)->RemoveWorker(_toSub);
	else if (allWorker_SideJobPool.Contains(previousowner))
		allWorker_SideJobPool.Find(previousowner)->RemoveWorker(_toSub);
	else if (allWorker_AssignedPool.Contains(previousowner))
		allWorker_AssignedPool.Find(previousowner)->RemoveWorker(_toSub);
}

void AWorkerWorldManager::SubscribeToAssignedPool(AWorker* _toSub)
{
	EPlayerIdent currentowner = _toSub->GetCurrentOwner();

	if (currentowner == EPlayerIdent::PI_DEFAULT)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorkerWorldManager, SubscribeToAssignedPool, currentowner was default"));
		return;
	}

	allWorker_AssignedPool.Find(currentowner)->AddWorker(_toSub);

	if (allWorker_UnassignedPool.Contains(currentowner))
		allWorker_UnassignedPool.Find(currentowner)->RemoveWorker(_toSub);
	else if (allWorker_WorldPool.Contains(currentowner))
		allWorker_WorldPool.Find(currentowner)->RemoveWorker(_toSub);
	else if (allWorker_SideJobPool.Contains(currentowner))
		allWorker_SideJobPool.Find(currentowner)->RemoveWorker(_toSub);
}

void AWorkerWorldManager::AddProductionSite(AProductionsite* _toadd)
{
	if (!allProductionSites.Contains(_toadd))
		allProductionSites.Add(_toadd);
}
