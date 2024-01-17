// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"

#include "Worker.h"

// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	transportManager = CreateDefaultSubobject<UTransportManager>("TransportManager");
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();


	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerBase::TickWorkerPayement, payementTickValue);
}

void APlayerBase::TickWorkerPayement()
{
	if(!productionSiteManager)
	{
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerBase::TickWorkerPayement, payementTickValue);

		UE_LOG(LogTemp,Warning,TEXT("APlayerBase, !productionSiteManager"));
		return;
	}

	if (productionSiteManager->GetAllHiredWorker().Num() <= 0 && productionSiteManager->GetAllUnasignedWorker().Num() <= 0)
	{
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerBase::TickWorkerPayement, payementTickValue);

		return;
	}

	TArray<AWorker*> workerstopay;

	for(AWorker* worker : productionSiteManager->GetAllHiredWorker())
	{
		if (!workerstopay.Contains(worker))
			workerstopay.Add(worker);
	}

	for (AWorker* worker : productionSiteManager->GetAllUnasignedWorker())
	{
		if (!workerstopay.Contains(worker))
			workerstopay.Add(worker);
	}

	for (AWorker* worker : workerstopay)
	{
		if(playerCurrency - worker->GetWorkerSalary() > 0)
		{
			playerCurrency -= worker->GetWorkerSalary();
			worker->AddCurrencyToWorker(worker->GetWorkerSalary());
		}
		else
		{
			if(playerCurrency > 0)
				worker->AddCurrencyToWorker(playerCurrency);
			else
				worker->AddCurrencyToWorker(0);
		}
	}

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerBase::TickWorkerPayement, payementTickValue);
}

void APlayerBase::AddOrDeductCurrency(float _amount)
{
	playerCurrency += _amount;
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FPlayerSaveData APlayerBase::GetPlayerSaveData()
{
	return FPlayerSaveData();
}

void APlayerBase::InitPlayerStart(FPlayerSaveData _saveData, AMarketManager* _marketManager, AWorkerWorldManager* _workerWorldManager)
{
	playerIdent = _saveData.GetIdent_S();
}

UProductionSiteManager* APlayerBase::GetProductionSiteManager()
{
	return productionSiteManager;
}

AWorkerWorldManager* APlayerBase::GetWorkerWorldManager()
{
	return workerWorldManager;
}