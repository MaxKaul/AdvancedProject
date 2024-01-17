// Fill out your copyright notice in the Description page of Project Settings.


#include "Worker.h"

#include "NavigationSystem.h"
#include "Productionsite.h"
#include "WorkerController.h"
#include "WorkerMoodManager.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
AWorker::AWorker()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	productivity = 0.08f;

	ZOffsetForSites = 700.f;

	interactionRangeBuildings = 200.f;
	interactionRangeStalls = 200.f;

	defaultWeightDivider = 1000.f;

	desireDefaultMax = 100.f;
	externalMoralModifier = 1.f;

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
	desireLuxury = _saveData.GetDesireLuxury_S();
	desireNutrition = _saveData.GetDesireNutrition_S();
	desireDefaultMax = _saveData.GetDesireDefaultMax_S();
	currentWeightLuxury = _saveData.GetWeightLuxury_S();
	currentWeightNutrition = _saveData.GetWeightNutrition_S();
	ownedCurrency = _saveData.GetOwnedCurrency_S();

	possibleStallIDs = _possibleStallsIDs;
	InitPossibleStallsFromSave();

	workerOptionals.workProductionSiteID = _saveData.GetWorkerOptionals_S().workProductionSiteID;

	skeletalMeshComponent = GetMesh();
	world = GetWorld();
	characterMovementComp = GetCharacterMovement();

	skeletalMesh = _saveData.GetWorkerMesh_S();

	skeletalMeshComponent->SetSkeletalMesh(skeletalMesh, true);
	workerController = Cast<AWorkerController>(GetController());
	navigationSystem = _navSystem;
	workerOwner = _workerOwner;
	workerDesireBases = _desireBase;

	FillBiasLists();

	if (currentLuxuryGood == EResourceIdent::ERI_DEFAULT || currentNutritionGood == EResourceIdent::ERI_DEFAULT)
		ChoseCurrentBiasWeightPair();
}

void AWorker::InitPossibleStallsFromSave()
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
	int prodid = 0;

	if (workerOptionals.workProductionSiteID.IsSet())
		prodid = workerOptionals.workProductionSiteID.GetValue();


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
		desireDefaultMax,
		desireLuxury,
		desireNutrition,
		possibleStallIDs,
		currentWeightNutrition,
		currentWeightLuxury,
		ownedCurrency,
		prodid
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
		subbedSite = _site;
		workerOptionals.workProductionSiteID = subbedSite->GetLocalProdSiteID();
	}
	else
		subbedSite = nullptr;
}

void AWorker::State_Idle()
{
	if (bIsDebugActive || GetMovementComponent()->Velocity.Length() <= 0)
	{
		FNavLocation rndloc;

		navigationSystem->GetRandomPoint(rndloc);

		workerController->MoveToLocation(rndloc);

		// Ich Invalidiere den debugger im state da ich keinen state change habe wenn die conditions aus erfüllt sind
		InvalidateDebugTimer();
		SetDebugTimer(rndloc);
	}
}

void AWorker::State_Employed_Unassigned()
{
	if (bIsDebugActive || GetMovementComponent()->Velocity.Length() <= 0)
	{
		FNavLocation rndloc;

		navigationSystem->GetRandomPoint(rndloc);

		workerController->MoveToLocation(rndloc);

		// Ich Invalidiere den debugger im state da ich keinen state change habe wenn die conditions aus erfüllt sind
		InvalidateDebugTimer();
		SetDebugTimer(rndloc);
	}
}

void AWorker::State_Employed_SideJob()
{

}

void AWorker::State_Employed_Assigned()
{
	if (!subbedSite || bIsCachedInSite)
		return;

	FVector loc = subbedSite->GetInteractionPos();
	//loc.Z += ZOffsetForSites;

	workerController->MoveToLocation(loc);

	SetDebugTimer(loc);

	if (bIsDebugActive || (loc - this->GetActorLocation()).Length() <= interactionRangeBuildings)
	{
		if ((loc - this->GetActorLocation()).Length() > interactionRangeBuildings)
			SetActorLocation(loc);

		subbedSite->SubscribeWorkerToOnSite(this);

		// Ich Invalidiere hier nur den timer da mit dem erfüllen der conditions in diesem state keien weitere movement order vorhanden sein wird und ein state change einen neuen timer triggern würde
		InvalidateDebugTimer();
	}
}

void AWorker::State_FulfillingNeed()
{
	if (workerOptionals.possibleMarketStalls.IsSet() && bProcessingTicket)
	{
		if (bIsDebugActive || (GetActorLocation() - movePos).Length() <= interactionRangeStalls)
		{
			BuyResources();

			// Ich Invalidiere hier nur den timer da mit dem erfüllen der conditions in diesem state keien weitere movement order vorhanden sein wird und ein state change einen neuen timer triggern würde
			InvalidateDebugTimer();
		}
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

		UE_LOG(LogTemp,Warning,TEXT("DDDDD"))

		movePos = workerOptionals.currentMarketStall.GetValue()->GetMesh()->GetSocketLocation(possibleSockets[rndsocket]);
		MoveWorker(movePos);

		bProcessingTicket = true;

		InvalidateDebugTimer();
		SetDebugTimer(movePos);
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
	rndidx = FMath::RandRange(0, luxuryBiasWeightPair.Num() - 1);

	for (TTuple<EResourceIdent, float> chosenattribute : luxuryBiasWeightPair)
	{
		if (idx == rndidx)
		{
			currentLuxuryGood = chosenattribute.Key;
			currentWeightLuxury = chosenattribute.Value;
			break;
		}
		else
			idx++;
	}

	rndidx = FMath::RandRange(0, nutritionBiasWeightPair.Num() - 1);
	idx = 0;

	for (TTuple<EResourceIdent, float> chosenattribute : nutritionBiasWeightPair)
	{
		if (idx == rndidx)
		{
			currentNutritionGood = chosenattribute.Key;
			currentWeightNutrition = chosenattribute.Value;
			break;
		}
		else
			idx++;
	}
}

void AWorker::State_SomethingWentWrong()
{
	UE_LOG(LogTemp, Warning, TEXT("AWorker, State_SomethingWentWrong, UFF"));
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

	float fulfillmentvalue = marketManager->GetPoolInfo().FindRef(ident).GetFulfillmentValue();
	float resourcevalue = marketManager->GetPoolInfo().FindRef(ident).GetLastResourcePrice();

	buyamount = (int)(desireDefaultMax / fulfillmentvalue);

	if (buyamount <= 0)
		buyamount = 1;

	if (marketManager->GetPoolInfo().FindRef(ident).GetResourceAmount() < buyamount)
		buyamount = marketManager->GetPoolInfo().FindRef(ident).GetResourceAmount();

	if(ownedCurrency < (buyamount * resourcevalue))
	{
		int iteratenum = buyamount;

		for (size_t i = 0; i < iteratenum; i++)
		{
			buyamount--;

			if (ownedCurrency >= (buyamount * resourcevalue))
			{
				capital = buyamount * resourcevalue;
				break;
			}
		}
	}
	else
		capital = buyamount * resourcevalue;

	ownedCurrency -= capital;

	float maxBuyPricePerResource = 99999.f;
	float minSellPricePerResource = 0.f;

	return FResourceTransactionTicket(buyamount, capital, ident, maxBuyPricePerResource, minSellPricePerResource);
}

FResourceTransactionTicket AWorker::CalculateTicket_Nutrition()
{
	int buyamount = 0;
	float capital = 0;
	EResourceIdent ident = currentNutritionGood;

	float fulfillmentvalue = marketManager->GetPoolInfo().FindRef(ident).GetFulfillmentValue();
	float resourcevalue = marketManager->GetPoolInfo().FindRef(ident).GetLastResourcePrice();

	buyamount = (int)(desireDefaultMax / fulfillmentvalue);

	if (buyamount <= 0)
		buyamount = 1;

	if (marketManager->GetPoolInfo().FindRef(ident).GetResourceAmount() < buyamount)
		buyamount = marketManager->GetPoolInfo().FindRef(ident).GetResourceAmount();

	if (ownedCurrency < (buyamount * resourcevalue))
	{
		int iteratenum = buyamount;

		for (size_t i = 0; i < iteratenum; i++)
		{
			buyamount--;

			if (ownedCurrency >= (buyamount * resourcevalue))
			{
				capital = buyamount * resourcevalue;
				break;
			}
		}
	}
	else
		capital = buyamount * resourcevalue;

	ownedCurrency -= capital;

	float maxBuyPricePerResource = 99999.f;
	float minSellPricePerResource = 0.f;

	return FResourceTransactionTicket(buyamount, capital, ident, maxBuyPricePerResource, minSellPricePerResource);
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
	InvalidateDebugTimer();
}


void AWorker::MoveWorker(FVector _movePos)
{
	workerController->MoveToLocation(_movePos);
}

void AWorker::TickWorkerDesire()
{
	if (desireLuxury > 0)
		desireLuxury -= currentWeightLuxury / defaultWeightDivider;
	
	if (desireNutrition > 0)
		desireNutrition -= currentWeightNutrition / defaultWeightDivider;
	
	if(desireLuxury <= 0 && desireNutrition <= 0)
		SetWorkerState(EWorkerStatus::WS_FullfillNeed);
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
}
	
void AWorker::UncacheWorkerFromSite()
{
	capsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	workerHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetVisibility(true);
	bIsCachedInSite = false;
}

void AWorker::CacheWorkerToSite()
{
	capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	workerHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetVisibility(false);
	workerController->StopMovement();
	bIsCachedInSite = true;
}

bool AWorker::NullcheckDependencies()
{
	bool status = true;

	return status;
}

void AWorker::SetDebugTimer(FVector _endPos)
{
	if (debugTimerHandle.IsValid())
		return;

	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorker, !world"));
		return;
	}

	if(!characterMovementComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWorker, !characterMovementComp"));
		return;
	}

	double pathlenght = 0;
	UNavigationSystemV1::GetPathLength(world, GetActorLocation(), _endPos, pathlenght);
	pathlenght = (float)pathlenght;


	float speed = characterMovementComp->GetMaxSpeed();

	float time = pathlenght / speed;

	time += debugAddTime;

	DrawDebugSphere(world, _endPos, 16, 16, FColor::Red, true, -1);

	world->GetTimerManager().SetTimer(debugTimerHandle,this , &AWorker::ActivateDebugAction, time, false);


}

void AWorker::ActivateDebugAction()
{
	bIsDebugActive = true;

	UE_LOG(LogTemp,Warning,TEXT("Debug Action was Activated form %s"), *this->GetName())
}

void AWorker::InvalidateDebugTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("InvalidateDebugTimer"));
	bIsDebugActive = false;
	world->GetTimerManager().ClearTimer(debugTimerHandle);
}