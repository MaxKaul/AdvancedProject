// Fill out your copyright notice in the Description page of Project Settings.


#include "Worker.h"

#include "NavigationSystem.h"
#include "Productionsite.h"
#include "WorkerController.h"
#include "WorkerMoodManager.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
AWorker::AWorker()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	productivity = 0.08f;

	ZOffsetForSites = 700.f;
	interactionRange = 200.f;

	desireDefaultMax = 100.f;

	workerHitBox = CreateDefaultSubobject<UCapsuleComponent>("Hit Box");
	workerHitBox->SetupAttachment(GetRootComponent());

	moodManager = CreateDefaultSubobject<UWorkerMoodManager>("Mood Manager");

	capsuleComp = GetCapsuleComponent();

	possibleSockets.Add("InteractionSocket_0");
	possibleSockets.Add("InteractionSocket_1");
	possibleSockets.Add("InteractionSocket_2");
}

// Called when the game starts or when spawned
void AWorker::BeginPlay()
{
	Super::BeginPlay();

}

void AWorker::OnOverlapBegin(UPrimitiveComponent* _overlapComp, AActor* _otherActor, UPrimitiveComponent* _otherComp,
	int32 _otherBodyIdx, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (subbedSite && currentStatus == EWorkerStatus::WS_Assigned_MainJob && Cast<AProductionsite>(_otherActor) == subbedSite)
		subbedSite->SubscribeWorkerToOnSite(this);
}

void AWorker::OnOverlapEnd(UPrimitiveComponent* _overlapComp, AActor* _otherActor, UPrimitiveComponent* _otherComp,
	int32 _otherBodyIdx)
{

}

// Called every frame
void AWorker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (currentStatus)
	{
	case EWorkerStatus::WS_Unemployed:
		State_Idle();
		break;
	case EWorkerStatus::WS_Assigned_SideJob:
		State_Employed_Assigned();
		break;
	case EWorkerStatus::WS_Unassigned:
		State_Employed_Unassigned();
		break;
	case EWorkerStatus::WS_Assigned_MainJob:
		State_Employed_Assigned();
		break;

	case EWorkerStatus::WS_FullfillNeed:
			State_FulfillingNeed();
		break;

	case EWorkerStatus::WS_DEFAULT:
		break;
	case EWorkerStatus::WS_ENTRY_AMOUNT:
		State_SomethingWentWrong();
		break;
	default:
		State_SomethingWentWrong();
		break;;
	}

}

void AWorker::InitWorker(FWorkerSaveData _saveData, UNavigationSystemV1* _navSystem, int _workerID, EWorkerStatus _employementStatus, int _siteID, EPlayerIdent _workerOwner, TArray<FWorkerDesireBase> _desireBase, AMarketManager* _marketManager)
{
	workerID = _workerID;
	workerOptionals.productionSiteID  = _siteID;
	currentStatus = _employementStatus;
	marketManager = _marketManager;

	skeletalMeshComponent = GetMesh();
	
	skeletalMesh = _saveData.GetWorkerMesh();

	skeletalMeshComponent->SetSkeletalMesh(skeletalMesh, true);
	workerController = Cast<AWorkerController>(GetController());
	navigationSystem = _navSystem;
	workerOwner = _workerOwner;
	workerDesireBases = _desireBase;

	workerHitBox->OnComponentBeginOverlap.AddDynamic(this, &AWorker::OnOverlapBegin);
	workerHitBox->OnComponentEndOverlap.AddDynamic(this, &AWorker::OnOverlapEnd);

	FillBiasLists();

	FTimerHandle handle;

	GetWorld()->GetTimerManager().SetTimer(handle, this, &AWorker::DEBUGSTARTDESIRE, 3.f);
}

FWorkerSaveData AWorker::GetWorkerSaveData()
{
	FWorkerSaveData savedata =
	{
		GetActorLocation(),
		GetActorRotation(),
		skeletalMesh,
		workerID,
		currentStatus,
		workerOptionals.productionSiteID.GetValue(),
		workerOwner,
		workerDesireBases
	};

	return savedata;
}

void AWorker::SetWorkerState(EWorkerStatus _employmentStatus, AProductionsite* _site)
{
	if (currentStatus == EWorkerStatus::WS_FullfillNeed && _employmentStatus != EWorkerStatus::WS_FinishFullfillNeed)
	{
		if (_employmentStatus == EWorkerStatus::WS_Unemployed || _employmentStatus == EWorkerStatus::WS_Unassigned || _employmentStatus == EWorkerStatus::WS_Assigned_MainJob)
			previousStatus = _employmentStatus;

		return;
	}
	else if(_employmentStatus == EWorkerStatus::WS_FinishFullfillNeed)
	{
		currentStatus = previousStatus;
	}
	else
	{
		previousStatus = currentStatus;
		currentStatus = _employmentStatus;
	}


	workerOptionals.possibleMarketStalls.Reset();
	workerOptionals.chosenMarketStall.Reset();
	movePos = FVector(0);

	workerController->StopMovement();

	if (_site && currentStatus == EWorkerStatus::WS_Assigned_MainJob)
		subbedSite = _site;
	else
		subbedSite = nullptr;
}

void AWorker::State_Idle()
{
	if (GetMovementComponent()->Velocity.Length() > 0)
		return;

	FNavLocation rndloc;
	navigationSystem->GetRandomPoint(rndloc);

	workerController->MoveToLocation(rndloc);
}

void AWorker::State_Employed_Unassigned()
{
	if (GetMovementComponent()->Velocity.Length() > 0)
		return;

	FNavLocation rndloc;
	navigationSystem->GetRandomPoint(rndloc);

	workerController->MoveToLocation(rndloc);
}

void AWorker::State_Employed_SideJob()
{

}

void AWorker::State_Employed_Assigned()
{
	if (!subbedSite)
		return;

	FVector loc = subbedSite->GetActorLocation();
	loc.Z += ZOffsetForSites;

	workerController->MoveToLocation(loc);
}

void AWorker::State_FulfillingNeed()
{
	if (workerOptionals.chosenMarketStall.IsSet())
	{
		if ((GetActorLocation() - movePos).Length() <= interactionRange)
			BuyResources();

		return;
	}
	else if (!workerOptionals.possibleMarketStalls.IsSet())
		workerOptionals.possibleMarketStalls = ChooseMarketStalls();
	else if (!workerOptionals.possibleMarketStalls.IsSet())
	{
		UE_LOG(LogTemp,Warning,TEXT("AWorker, Could not Choose Market Stall"))
		return;
	}

	int rndidx = 0;
	int rndsocket = FMath::RandRange(0, possibleSockets.Num() - 1);

	if (workerOptionals.possibleMarketStalls.GetValue().Num() > 0)
		rndidx = FMath::RandRange(0, workerOptionals.possibleMarketStalls.GetValue().Num() - 1);

	workerOptionals.chosenMarketStall = workerOptionals.possibleMarketStalls.GetValue()[rndidx];

	movePos = workerOptionals.possibleMarketStalls.GetValue()[rndidx]->GetMesh()->GetSocketLocation(possibleSockets[rndsocket]);

	MoveWorker(movePos);
}

TArray<AMarketStall*> AWorker::ChooseMarketStalls()
{
	TArray<AMarketStall*> possiblestalls;

	// rnd für die individuelle order


	int rndidx = 0;

	rndidx = FMath::RandRange(0, allLuxurybBiases.Num() - 1);
	currentLuxuryGood = allLuxurybBiases[rndidx];

	rndidx = FMath::RandRange(0, allNutritionBiases.Num() - 1);
	currentNutritionGood = allNutritionBiases[rndidx];

	for(AMarketStall* stall : marketManager->GetAllMarketStalls())
	{
		TArray<EResourceIdent> availableresoruces;

		if (stall->GetStallResources().Contains(currentLuxuryGood) || stall->GetStallResources().Contains(currentNutritionGood))
			possiblestalls.Add(stall);
	}

	return possiblestalls;
}

void AWorker::State_SomethingWentWrong()
{
	UE_LOG(LogTemp, Warning, TEXT("UFF"));
}

void AWorker::BuyResources()
{
	TArray<FResourceTransactionTicket> buyorder;

	if(workerOptionals.chosenMarketStall.GetValue()->GetStallResources().Contains(currentLuxuryGood))
	{
		FResourceTransactionTicket ticket_luxury = CalculateTicket_Luxury();

		buyorder.Add(ticket_luxury);
	}

	if (workerOptionals.chosenMarketStall.GetValue()->GetStallResources().Contains(currentNutritionGood))
	{
		FResourceTransactionTicket ticket_nutrition = CalculateTicket_Nutrition();

		buyorder.Add(ticket_nutrition);
	}

	ProcessBuyReturn(marketManager->BuyResources(buyorder));
}


FResourceTransactionTicket AWorker::CalculateTicket_Luxury()
{
	int buyamount = 0;
	float capital = 0;
	EResourceIdent ident = currentLuxuryGood;
	FMarketManagerOptionals mmoptionals;

	float desiretofill = desireDefaultMax - desireLuxury;
	float fulfillmentvalue = marketManager->GetPoolInfo().FindRef(ident).GetFulfillmentValue();
	float resourcevalue = marketManager->GetPoolInfo().FindRef(ident).GetLastResourcePrice();

	buyamount = (int)(desiretofill / resourcevalue);

	if (marketManager->GetPoolInfo().FindRef(ident).GetResourceAmount() < buyamount)
		buyamount = marketManager->GetPoolInfo().FindRef(ident).GetResourceAmount();

	if(ownedCurrency < (buyamount * resourcevalue))
	{
		for (size_t i = 0; i < buyamount; i++)
		{
			buyamount--;

			if (ownedCurrency >= (buyamount * resourcevalue))
				break;
		}
	}

	capital = buyamount * resourcevalue;

	mmoptionals.maxBuyPricePerResource = 99999.f;
	mmoptionals.minSellPricePerResource = 0.f;

	return FResourceTransactionTicket(buyamount, capital, ident, mmoptionals.maxBuyPricePerResource, mmoptionals.maxBuyPricePerResource);
}

FResourceTransactionTicket AWorker::CalculateTicket_Nutrition()
{
	int buyamount = 0;
	float capital = 0;
	EResourceIdent ident = currentNutritionGood;
	FMarketManagerOptionals mmoptionals;

	float desiretofill = desireDefaultMax - desireHunger;
	float fulfillmentvalue = marketManager->GetPoolInfo().FindRef(ident).GetFulfillmentValue();
	float resourcevalue = marketManager->GetPoolInfo().FindRef(ident).GetLastResourcePrice();

	buyamount = (int)(desiretofill / resourcevalue);

	if (marketManager->GetPoolInfo().FindRef(ident).GetResourceAmount() < buyamount)
		buyamount = marketManager->GetPoolInfo().FindRef(ident).GetResourceAmount();

	if (ownedCurrency < (buyamount * resourcevalue))
	{
		for (size_t i = 0; i < buyamount; i++)
		{
			buyamount--;

			if (ownedCurrency >= (buyamount * resourcevalue))
				break;
		}
	}
	else
		capital = buyamount * resourcevalue;

	ownedCurrency -= capital;

	mmoptionals.maxBuyPricePerResource = 99999.f;
	mmoptionals.minSellPricePerResource = 0.f;

	return FResourceTransactionTicket(buyamount, capital, ident, mmoptionals.maxBuyPricePerResource, mmoptionals.maxBuyPricePerResource);
}

void AWorker::ProcessBuyReturn(TArray<FResourceTransactionTicket> _tickets)
{
	for (FResourceTransactionTicket ticket : _tickets)
	{
		ownedCurrency += ticket.exchangedCapital;
		
		float fulfillmentvalue = marketManager->GetPoolInfo().FindRef(ticket.resourceIdent).GetFulfillmentValue();

		for (size_t i = 0; i < ticket.resourceAmount; i++)
		{
			if(ticket.resourceIdent == currentLuxuryGood)
				desireLuxury += fulfillmentvalue;
			else if(ticket.resourceIdent == currentNutritionGood)
				desireHunger += fulfillmentvalue;
		}
	}

	FinishFulfillNeed();
}

void AWorker::FinishFulfillNeed()
{
	SetWorkerState(EWorkerStatus::WS_FinishFullfillNeed);
}


void AWorker::MoveWorker(FVector _movePos)
{
	if (GetMovementComponent()->Velocity.Length() > 0)
		return;

	workerController->MoveToLocation(_movePos);
}

void AWorker::FillBiasLists()
{
	for (FWorkerDesireBase base : workerDesireBases)
	{
		for (size_t i = 0; i < base.GetDesired_LuxuryResources().Num(); i++)
		{
			allLuxurybBiases.Add(base.GetDesired_LuxuryResources()[i]);
		}

		for (size_t i = 0; i < base.GetDesired_NutritionResources().Num(); i++)
		{
			allNutritionBiases.Add(base.GetDesired_NutritionResources()[i]);
		}
	}
}

void AWorker::DEBUGSTARTDESIRE()
{
	SetWorkerState(EWorkerStatus::WS_FullfillNeed);
}

void AWorker::UncacheWorkerFromSite()
{
	capsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	workerHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetVisibility(true);
}

void AWorker::CacheWorkerToSite()
{
	capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	workerHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetVisibility(false);
	workerController->StopMovement();
}

bool AWorker::NullcheckDependencies()
{
	bool status = true;

	return status;
}

int AWorker::GetWorkerID() {return workerID;}