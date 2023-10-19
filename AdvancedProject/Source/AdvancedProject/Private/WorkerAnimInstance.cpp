// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkerAnimInstance.h"

#include "Worker.h"

void UWorkerAnimInstance::NativeBeginPlay()
{
	Super::NativeInitializeAnimation();

	instanceOwner = Cast<AWorker>(TryGetPawnOwner()->GetController());
}

void UWorkerAnimInstance::UpdateAnimationProperties()
{
	if (!instanceOwner)
		return;

	velocity = instanceOwner->GetVelocity().Length();
	bIsWorking = instanceOwner->GetIsWorking();
}
