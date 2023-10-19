// Fill out your copyright notice in the Description page of Project Settings.


#include "Worker.h"

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

// Called to bind functionality to input
void AWorker::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AWorker::InitWorker(FWorkerSaveData _saveData)
{
	skeletalMeshComponent = GetMesh();

	skeletalMesh = _saveData.GetWorkerMesh();

	skeletalMeshComponent->SetSkeletalMesh(skeletalMesh, true);
	workerController = Cast<AWorkerController>(GetController());

	SetActorLocation(_saveData.GetWorkerLocation());
	SetActorRotation(_saveData.GetWorkerRotation());
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
