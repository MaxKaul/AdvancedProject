// Fill out your copyright notice in the Description page of Project Settings.


#include "Worker.h"

#include "NavigationSystem.h"
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

void AWorker::InitWorker(FWorkerSaveData _saveData, UNavigationSystemV1* _navSystem)
{
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
		skeletalMesh
	};

	return savedata;
}

bool AWorker::NullcheckDependencies()
{
	bool status = true;

	return status;
}
