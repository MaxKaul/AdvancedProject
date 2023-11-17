// Fill out your copyright notice in the Description page of Project Settings.


#include "Worker.h"

#include "NavigationSystem.h"
#include "Productionsite.h"
#include "WorkerController.h"

// Sets default values
AWorker::AWorker()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	productivity = 0.08f;
}

// Called when the game starts or when spawned
void AWorker::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWorker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int AWorker::GetWorkerID()
{
	return workerID;
}

void AWorker::InitWorker(FWorkerSaveData _saveData, UNavigationSystemV1* _navSystem, int _workerID, EWorkerStatus _employementStatus, int _siteID)
{
	workerID = _workerID;
	workerOptionals.productionSiteID  = _siteID;
	employmentStatus = _employementStatus;

	skeletalMeshComponent = GetMesh();
	
	skeletalMesh = _saveData.GetWorkerMesh();

	skeletalMeshComponent->SetSkeletalMesh(skeletalMesh, true);
	workerController = Cast<AWorkerController>(GetController());
	navigationSystem = _navSystem;

	FNavLocation testloc;

	navigationSystem->GetRandomPoint(testloc);

	workerController->MoveToLocation(testloc.Location);
}

FWorkerSaveData AWorker::GetWorkerSaveData()
{
	FWorkerSaveData savedata =
	{
		GetActorLocation(),
		GetActorRotation(),
		skeletalMesh,
		workerID,
		employmentStatus,
		workerOptionals.productionSiteID.GetValue()
	};

	return savedata;
}

void AWorker::SetEmployementStatus(EWorkerStatus _employmentStatus, AProductionsite* _site)
{
	employmentStatus = _employmentStatus;

	if (_site)
		subbedSite = _site;
}

bool AWorker::NullcheckDependencies()
{
	bool status = true;

	return status;
}
