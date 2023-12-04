// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkerController.h"

#include "Navigation/CrowdFollowingComponent.h"

AWorkerController::AWorkerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}