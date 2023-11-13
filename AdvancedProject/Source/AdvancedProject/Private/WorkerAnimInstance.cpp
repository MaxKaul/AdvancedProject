// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkerAnimInstance.h"

#include "Worker.h"

void UWorkerAnimInstance::NativeBeginPlay()
{
	Super::NativeInitializeAnimation();

	instanceOwner = Cast<AWorker>(TryGetPawnOwner());
}

void UWorkerAnimInstance::UpdateAnimationProperties()
{
	if (!instanceOwner)
		return;

	velocity = instanceOwner->GetVelocity().Length();

	// Müsste noch etwas besser gemacht werden
	if (instanceOwner->GetEmployementStatus() == EWorkerStatus::WS_Employed_MainJob ||
		instanceOwner->GetEmployementStatus() == EWorkerStatus::WS_Employed_SideJob)
		bIsWorking = true;
	else
		bIsWorking = false;
}
