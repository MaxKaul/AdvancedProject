// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingSite.h"

#include "Components/SphereComponent.h"

// Sets default values
ABuildingSite::ABuildingSite()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	actorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Actor Mesh");
	RootComponent = actorMeshComponent;

	interactionPoint = CreateDefaultSubobject<USphereComponent>("Interaction Point");
	interactionPoint->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ABuildingSite::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABuildingSite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABuildingSite::SetBuildStatus(bool _wasBuildOn)
{
	bHasBeenBuildOn = _wasBuildOn;

	if (bHasBeenBuildOn)
		actorMeshComponent->SetMaterial(0, translucentMat);
}

