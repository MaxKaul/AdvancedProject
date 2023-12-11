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
	externalMoralModifier = 1.f;

	workerHitBox = CreateDefaultSubobject<UCapsuleComponent>("Hit Box");
	workerHitBox->SetupAttachment(GetRootComponent());

	moodManager = CreateDefaultSubobject<UWorkerMoodManager>("Mood Manager");

	capsuleComp = GetCapsuleComponent();

	possibleSockets.Add("InteractionSocket_0");
	possibleSockets.Add("InteractionSocket_1");
	possibleSockets.Add("InteractionSocket_2");

	DEBUG = 0;
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

	TickWorkerDesire();
	TickWorkerMotivation();
	
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

void AWorker::InitWorker(FWorkerSaveData _saveData, UNavigationSystemV1* _navSystem, int _workerID, EWorkerStatus _employementStatus, EWorkerStatus _cachedStatus, EPlayerIdent _workerOwner, TArray<FWorkerDesireBase> _desireBase, 
	AMarketManager* _marketManager, TArray<int> _possibleStallsIDs)
{
	workerID = _workerID;
	currentStatus = _employementStatus;
	previousStatus = _cachedStatus;
	marketManager = _marketManager;
	currentLuxuryGood = _saveData.GetCurrentLuxuryGood_S();
	currentNutritionGood = _saveData.GetCurrentNutritionGood_S();
	desireLuxury = _saveData.GetDesireLuxury();
	desireNutrition = _saveData.GetDesireHunger();
	desireDefaultMax = _saveData.GetDesireDefaultMax();

	//if (_saveData.GetWorkerOptionals_S().possibleStallIDs.IsSet() && _saveData.GetWorkerOptionals_S().possibleStallIDs.GetValue().Num() > 0)
	//{
	//	workerOptionals.possibleStallIDs = _saveData.GetWorkerOptionals_S().possibleStallIDs.GetValue();
	//	InitPossibleSitesFromSave();
	//}

	possibleStallIDs = _possibleStallsIDs;
	InitPossibleSitesFromSave();

	workerOptionals.workProductionSiteID = _saveData.GetWorkerOptionals_S().workProductionSiteID;

	skeletalMeshComponent = GetMesh();
	
	skeletalMesh = _saveData.GetWorkerMesh_S();

	skeletalMeshComponent->SetSkeletalMesh(skeletalMesh, true);
	workerController = Cast<AWorkerController>(GetController());
	navigationSystem = _navSystem;
	workerOwner = _workerOwner;
	workerDesireBases = _desireBase;

	workerHitBox->OnComponentBeginOverlap.AddDynamic(this, &AWorker::OnOverlapBegin);
	workerHitBox->OnComponentEndOverlap.AddDynamic(this, &AWorker::OnOverlapEnd);

	FillBiasLists();

	FTimerHandle handle;

	GetWorld()->GetTimerManager().SetTimer(handle, this, &AWorker::ChoseCurrentBiasWeightPair, 2.f, false);
}

void AWorker::InitPossibleSitesFromSave()
{
	if (!marketManager)
	{
		UE_LOG(LogTemp,Warning,TEXT("AWorker, !marketManager"))
		return;
	}

	TArray<AMarketStall*> allstalls = marketManager->GetAllMarketStalls();

	TArray<AMarketStall*> newarray;

	for (size_t i = 0; i < possibleStallIDs.Num(); i++)
	{
		for (AMarketStall* stall : marketManager->GetAllMarketStalls())
		{
			if (stall->GetStallID() == possibleStallIDs[i])
			{
				if(workerOptionals.possibleMarketStalls.IsSet())
					workerOptionals.possibleMarketStalls.GetValue().Add(stall);
				else
				{
					newarray.Add(stall);
					workerOptionals.possibleMarketStalls = newarray;
					
				}
			}
		}
	}
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
		previousStatus,
		workerOwner,
		workerDesireBases,
		currentLuxuryGood,
		currentNutritionGood,
		desireLuxury,
		desireNutrition,
		desireDefaultMax,
		possibleStallIDs,
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
	workerOptionals.currentMarketStall.Reset();
	movePos = FVector(0);

	workerController->StopMovement();

	if (_site && currentStatus == EWorkerStatus::WS_Assigned_MainJob)
	{
		workerOptionals.workProductionSiteID = subbedSite->GetLocalProdSiteID();
		subbedSite = _site;
	}
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
	if (workerOptionals.possibleMarketStalls.IsSet() && bProcessingTicket)
	{
		if ((GetActorLocation() - movePos).Length() <= interactionRange)
			BuyResources();
	}
	else if (!workerOptionals.possibleMarketStalls.IsSet() || !bProcessingTicket)
		workerOptionals.possibleMarketStalls = ChooseMarketStalls();
	else if (!workerOptionals.possibleMarketStalls.IsSet())
	{
		UE_LOG(LogTemp,Warning,TEXT("AWorker, Could not Choose Market Stall"))
		return;
	}

	if (!bProcessingTicket && workerOptionals.possibleMarketStalls.GetValue().Num() > 0)
	{
		int rndsocket = FMath::RandRange(0, possibleSockets.Num() - 1);

		workerOptionals.currentMarketStall = workerOptionals.possibleMarketStalls.GetValue()[0];

		for(AMarketStall* stall : workerOptionals.possibleMarketStalls.GetValue())
		{
			UE_LOG(LogTemp,Warning,TEXT("%i"), stall->GetStallID())
		}

		movePos = workerOptionals.currentMarketStall.GetValue()->GetMesh()->GetSocketLocation(possibleSockets[rndsocket]);
		MoveWorker(movePos);

		bProcessingTicket = true;
	}
	else if (workerOptionals.possibleMarketStalls.GetValue().Num() <= 0)
		FinishFulfillNeed();

	
}

TArray<AMarketStall*> AWorker::ChooseMarketStalls()
{
	TArray<AMarketStall*> possiblestalls;

	// rnd für die individuelle order
	ChoseCurrentBiasWeightPair();


	// Das mit den bools könnt ich unter umständen noch etwas schöbner machen
	bool bluxuryset = false;
	bool bnutritionset = false;

	for(AMarketStall* stall : marketManager->GetAllMarketStalls())
	{
		TArray<EResourceIdent> availableresoruces;

		if (stall->GetStallResources().Contains(currentLuxuryGood) || stall->GetStallResources().Contains(currentNutritionGood))
		{
			if(stall->GetStallResources().Contains(currentLuxuryGood) && stall->GetStallResources().Contains(currentNutritionGood))
			{
				bluxuryset = true;
				bnutritionset = true;
			}
			else if(stall->GetStallResources().Contains(currentLuxuryGood) && !stall->GetStallResources().Contains(currentNutritionGood))
				bluxuryset = true;
			else if(!stall->GetStallResources().Contains(currentLuxuryGood) && stall->GetStallResources().Contains(currentNutritionGood))
				bnutritionset = true;

			possiblestalls.Add(stall);

			if (possibleStallIDs.Num() > 0)
				possibleStallIDs.Add(stall->GetStallID());
			else 
			{
				TArray<int> initarray;
				initarray.Add(stall->GetStallID());

				possibleStallIDs = initarray;
			}

			if (bluxuryset && bnutritionset)
				break;
		}
	}

	return possiblestalls;
}

void AWorker::ChoseCurrentBiasWeightPair()
{
	int rndidx = 0;
	int idx = 0;
	rndidx = FMath::RandRange(0, luxuryBiasWeightPair.Num());

	for (TTuple<EResourceIdent, float> chosenattribute : luxuryBiasWeightPair)
	{
		if (idx != rndidx)
		{
			idx++;
			continue;
		}

		currentLuxuryGood = chosenattribute.Key;
		currentWeightLuxury = chosenattribute.Value;

		break;
	}

	idx = 0;
	rndidx = FMath::RandRange(0, luxuryBiasWeightPair.Num());

	for (TTuple<EResourceIdent, float> chosenattribute : nutritionBiasWeightPair)
	{
		if (idx != rndidx)
		{
			idx++;
			continue;
		}

		currentNutritionGood = chosenattribute.Key;
		currentWeightNutrition = chosenattribute.Value;

		break;
	}

}

void AWorker::State_SomethingWentWrong()
{
	UE_LOG(LogTemp, Warning, TEXT("UFF"));
}

void AWorker::BuyResources()
{
	TArray<FResourceTransactionTicket> buyorder;

	if(workerOptionals.currentMarketStall.GetValue()->GetStallResources().Contains(currentLuxuryGood))
	{
		FResourceTransactionTicket ticket_luxury = CalculateTicket_Luxury();

		buyorder.Add(ticket_luxury);
	}

	if (workerOptionals.currentMarketStall.GetValue()->GetStallResources().Contains(currentNutritionGood))
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

	buyamount = (int)(desiretofill / fulfillmentvalue);

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
	else
		capital = buyamount * resourcevalue;

	ownedCurrency -= capital;

	mmoptionals.maxBuyPricePerResource = 99999.f;
	mmoptionals.minSellPricePerResource = 0.f;

	return FResourceTransactionTicket(buyamount, capital, ident, mmoptionals.maxBuyPricePerResource, mmoptionals.minSellPricePerResource);
}

FResourceTransactionTicket AWorker::CalculateTicket_Nutrition()
{
	int buyamount = 0;
	float capital = 0;
	EResourceIdent ident = currentNutritionGood;
	FMarketManagerOptionals mmoptionals;

	float desiretofill = desireDefaultMax - desireNutrition;
	float fulfillmentvalue = marketManager->GetPoolInfo().FindRef(ident).GetFulfillmentValue();
	float resourcevalue = marketManager->GetPoolInfo().FindRef(ident).GetLastResourcePrice();

	buyamount = (int)(desiretofill / fulfillmentvalue);

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

	return FResourceTransactionTicket(buyamount, capital, ident, mmoptionals.maxBuyPricePerResource, mmoptionals.minSellPricePerResource);
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
				desireNutrition += fulfillmentvalue;
		}
	}

	bProcessingTicket = false;


	if (workerOptionals.possibleMarketStalls.GetValue().Num() > 0 && workerOptionals.possibleMarketStalls.GetValue()[0])
		workerOptionals.possibleMarketStalls.GetValue().RemoveAt(0);
	else if(workerOptionals.possibleMarketStalls.GetValue().Num() <= 0)
		FinishFulfillNeed();
}

void AWorker::FinishFulfillNeed()
{
	SetWorkerState(EWorkerStatus::WS_FinishFullfillNeed);
}


void AWorker::MoveWorker(FVector _movePos)
{
	//if (GetMovementComponent()->Velocity.Length() > 0)
	//	return;

	workerController->MoveToLocation(_movePos);
}

void AWorker::TickWorkerDesire()
{
	if (desireLuxury > 0)
		desireLuxury -= currentWeightLuxury / 1000.f;

	if (desireNutrition > 0)
		desireNutrition -= currentWeightNutrition / 1000.f;
}

void AWorker::TickWorkerMotivation()
{
	// Ich setzte mir die gewichtung des totale kapital besitzt aus den gewichtungen für luxury und nutrition zusammen
	float capitalweight = currentWeightLuxury + currentWeightNutrition;
	// Mal schauen wie sinnvoll es ist den wert so herzuleiten
	// Dies wird dann auch davon abhängen über welche rreichweite ich meinen exterenal modifier mache
	float modifierweight = capitalweight / currentWorkerMoral;

	currentWorkerMoral = currentWeightLuxury * desireLuxury + 
						 currentWeightNutrition * desireNutrition + 
						 capitalweight * ownedCurrency + 
						 modifierweight * externalMoralModifier;
}

void AWorker::FillBiasLists()
{
	for (FWorkerDesireBase base : workerDesireBases)
	{
		for(TTuple<EResourceIdent, float> item : base.GetDesired_LuxuryWeightPairs())
		{
			luxuryBiasWeightPair.Add(item.Key, item.Value);
		}

		for (TTuple<EResourceIdent, float> item : base.GetDesired_NutritionWeightPairs())
		{
			nutritionBiasWeightPair.Add(item.Key, item.Value);
		}
	}

	//ChoseCurrentBiasWeightPair();
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