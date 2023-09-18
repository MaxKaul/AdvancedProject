
#include "MarketManager.h"
#include "MarketStall.h"


AMarketManager::AMarketManager()
{
	PrimaryActorTick.bCanEverTick = true;

}


void AMarketManager::BeginPlay()
{
	Super::BeginPlay();

	world = GetWorld();

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager, !NullcheckDependencies"))
		return;
	}

	InitMarketStalls();
}


void AMarketManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMarketManager::InitMarketStalls()
{
	for (size_t i = 0; i < stallPositions.Num(); i++)
	{
		AMarketStall* tospawn;
		FVector spawnpos =  stallPositions[i];
		tospawn = Cast<AMarketStall>(world->SpawnActor(marketStall, &spawnpos));

		if (tospawn)
		{
			tospawn->SetMesh(stallMeshes[i]);
			spawnedStalls.Add(tospawn);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("AMarketManager, !tospawn"))
	}
}

bool AMarketManager::NullcheckDependencies()
{
	bool status = false;

	if (stallPositions.Num() > 0)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager !stallPositions"));

	if (world)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("world !stallPositions"));

	if (marketStall)
		status = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("AMarketManager !marketStall"));

	return status;
}