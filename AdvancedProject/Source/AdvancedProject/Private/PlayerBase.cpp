// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"

#include "Builder.h"
#include "Worker.h"

// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	transportManager = CreateDefaultSubobject<UTransportManager>("TransportManager");
	builder = CreateDefaultSubobject<UBuilder>("Builder");
	productionSiteManager = CreateDefaultSubobject<UProductionSiteManager>("ProductionSiteManager");
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
	FPlayerSaveData savedata;

	savedata.InitPlayerSaveData(GetActorLocation(), GetActorRotation(), productionSiteManager->GetProductionSiteManagerSaveData(), playerIdent, transportManager->GetTransportManagerSaveData(), playerCurrency);

	return savedata;
}

void APlayerBase::InitPlayerStart(FPlayerSaveData _saveData, AMarketManager* _marketManager, AWorkerWorldManager* _workerWorldManager, TArray<ABuildingSite*> _allBuildingSites)
{
	playerIdent = _saveData.GetIdent_S();

	marketManager = _marketManager;
	workerWorldManager = _workerWorldManager;
	playerCurrency = _saveData.GetCurrency_S();

	SetActorLocation(_saveData.GetLoaction_S());
	SetActorRotation(_saveData.GetRotation_S());

	FTransportManagerSaveData testsavedata;

	builder->InitBuilder(productionSiteManager, marketManager, this, workerWorldManager);

	productionSiteManager->InitProductionSiteManager(this, _saveData.GetProductionSiteManagerSaveData_S(), marketManager, workerWorldManager);
	transportManager->InitTransportManager(this, _saveData.GeTransportManagerSaveData_S(), marketManager, productionSiteManager);

	allBuildingSites = _allBuildingSites;
}

UProductionSiteManager* APlayerBase::GetProductionSiteManager()
{
	return productionSiteManager;
}

AWorkerWorldManager* APlayerBase::GetWorkerWorldManager()
{
	return workerWorldManager;
}