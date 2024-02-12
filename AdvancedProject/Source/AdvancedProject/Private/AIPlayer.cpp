// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPlayer.h"

#include "AIStates.h"

AAIPlayer::AAIPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	states = CreateDefaultSubobject<UAIStates>("AIStates");
}


void AAIPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!states || !world)
	{
		UE_LOG(LogTemp,Warning,TEXT("AAIPlayer, !states || !world"))
		return;
	}

	switch (currentState)
	{
	case EPossibleAIStates::PAIS_Wait:
		states->State_Wait();
		
		break;
	case EPossibleAIStates::PAIS_BuyResources:
		states->State_BuyResources();
		SetNewState(EPossibleAIStates::PAIS_Wait);
		break;
	case EPossibleAIStates::PAIS_SellResources:
		states->State_SellResources();
		SetNewState(EPossibleAIStates::PAIS_Wait);
		break;

	case EPossibleAIStates::PAIS_TransportResources:
		states->State_TransportResources();
		SetNewState(EPossibleAIStates::PAIS_Wait);
		break;

	case EPossibleAIStates::PAIS_BuildSite:
		states->State_BuildSite();
		SetNewState(EPossibleAIStates::PAIS_Wait);
		break;

	case EPossibleAIStates::PAIS_FireWorker:
		states->State_FireWorker();
		SetNewState(EPossibleAIStates::PAIS_Wait);
		break;

	case EPossibleAIStates::PAIS_HireWorker:
		states->State_HireWorker();
		SetNewState(EPossibleAIStates::PAIS_Wait);
		break;

	case EPossibleAIStates::PAIS_AssignWorker:
		states->State_AssignWorker();
		SetNewState(EPossibleAIStates::PAIS_Wait);
		break;

	case EPossibleAIStates::PAIS_UnassignWorker:
		states->State_UnassignWorker();
		SetNewState(EPossibleAIStates::PAIS_Wait);
		break;

	case EPossibleAIStates::PAIS_DEFAULT:
	case EPossibleAIStates::PAIS_ENTRY_AMOUNT:
		default:
		UE_LOG(LogTemp,Warning,TEXT("AAIPlayer, AIS_DEFAULT || PAIS_ENTRY_AMOUNT || default"))
		break;
	}
}

void AAIPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAIPlayer::InitPlayerStart(FPlayerSaveData _saveData, AMarketManager* _marketManager,
	AWorkerWorldManager* _workerWorldManager, TArray<ABuildingSite*> _allBuildingSites)
{
	Super::InitPlayerStart(_saveData, _marketManager, _workerWorldManager, _allBuildingSites);

	world = GetWorld();

	states->InitState(this);

	currentState = EPossibleAIStates::PAIS_BuildSite;

	stateProbabilityPair =
	{
		{EPossibleAIStates::PAIS_BuyResources,0 },
		{EPossibleAIStates::PAIS_SellResources, 0},
		{EPossibleAIStates::PAIS_TransportResources,0 },
		{EPossibleAIStates::PAIS_BuildSite, 0},
		{EPossibleAIStates::PAIS_FireWorker, 0 },
		{EPossibleAIStates::PAIS_HireWorker, 0 },
		{EPossibleAIStates::PAIS_AssignWorker, 0 },
		{EPossibleAIStates::PAIS_UnassignWorker,	0 }
	};
	

	allProdSiteTypes =
	{
		EProductionSiteType::PST_Ambrosia,
		EProductionSiteType::PST_Meat,
		EProductionSiteType::PST_Fruits,
		EProductionSiteType::PST_Wheat,
		EProductionSiteType::PST_Hardwood_Resin,
		EProductionSiteType::PST_Gold_Iron,
		EProductionSiteType::PST_Furniture_Jewelry,
		EProductionSiteType::PST_Ambrosia,
	};						 	


	// DEBUG, WEIL NOCH NICHT ALLE SATETS FERTIG SIND
	validStatesToTick =
	{
		{EPossibleAIStates::PAIS_BuyResources,0 },
	    {EPossibleAIStates::PAIS_BuildSite, 0},
	    {EPossibleAIStates::PAIS_TransportResources, 0},
	};

	TickSamples();

	GenerateAIBiases();
	SetDecisionTimer();
}

void AAIPlayer::TickDecision()
{
	bool bcanresetself = true;

	// Etwas harcode aber fuck it, drecks woche
	// Alles AIDS hier
	// Der wirft hier nen error, könnte sein das das wegen meiner runtime manipulation ist, aber lässt das programm jetzt auch nicht abschmieren


	// Ich muss die Tick vill noch woanders hinwerfen, wenn ich sie durch currentTickStates ticke kann ich die values nicht readden
	TickSamples();
	// Ich mach das jetzt mal so, ich will jetzt nicht zu viel an fertiger logik ändern
	currentTickStates = validStatesToTick;

	for (TTuple<EPossibleAIStates, float> state : currentTickStates)
	{
		EPossibleAIStates stateident = state.Key;
		float statevalue = state.Value;

		if (stateident != EPossibleAIStates::PAIS_BuildSite)
			return;

		float newvalue = statevalue;
		newvalue += FMath::RandRange(decisionBaseValueMin, decisionBaseValueMax);
		float prefvalue = 1.f;


		if (float value = currentBehaviourBase.GetStatePreferences().Contains(stateident))
		{
			prefvalue *= value;
			newvalue += prefvalue;
		}

		//UE_LOG(LogTemp, Warning, TEXT("%f"), newvalue);

		stateProbabilityPair.Add(stateident, newvalue);

		if (newvalue >= decisionThreshold || stateident == EPossibleAIStates::PAIS_BuildSite && productionSiteManager->GetAllProductionSites().Num() < 2 && newvalue >= 2.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("State Change"));
			SetNewState(stateident);
			bcanresetself = false;
			break;
		}
	}

	// Update validStatesToTick
	for(TTuple<EPossibleAIStates, float> state : validStatesToTick)
	{
		float newvalue = stateProbabilityPair.FindRef(state.Key);

		validStatesToTick.Add(state.Key, newvalue);
	}

	if(bcanresetself)
		SetDecisionTimer();
}

// Muss ich vill nochmal ändern, der wird jetzt zwie mal gecalled wenn der state gewechselt wird, einmal vom statechange wenn if(currstate.Value >= decisionThreshold)
// Und einmal vom StateChange in der switch, da jeder state damit endet das die ki wieder in den WaitingState zurück geht
void AAIPlayer::SetDecisionTimer()
{
	if (world->GetTimerManager().GetTimerRemaining(tickHandle) > 0)
		return;

	// Keine ahnung warum, aber SetTimer klappt hier irgendwie net, wenn ich versuche eine UFunc zu binden
	//auto ticklambda = [this]()
	//{
	//	TickDecision();
	//};

	//FTimerDelegate  currdelegate;
	//currdelegate.BindLambda(ticklambda);


	FTimerHandle testhandle;

	float time = FMath::RandRange(decicionTickRateMin, decicionTickRateMax);

	world->GetTimerManager().SetTimer(testhandle, this, &AAIPlayer::TickDecision, time, false);
}

void AAIPlayer::SetNewState(EPossibleAIStates _newState)
{
	previousState = currentState;
	currentState = _newState;

	int idx = 0;

	// Iterated auch +1, dreck
	for (TTuple<EPossibleAIStates, float> updatestate : stateProbabilityPair)
	{
		stateProbabilityPair.Add(updatestate.Key, 0);

		idx++;

		if (idx >= stateProbabilityPair.Num())
			break;
	}

	SetDecisionTimer();
}

void AAIPlayer::GenerateAIBiases()
{
	if(!behaviourBaseTable)
	{
		UE_LOG(LogTemp,Warning,TEXT("AAIPlayer ,!behaviourBaseTable"))
		return;
	}

	TArray<FAIPlayerBehaviourTable*> tablerowref;

	for (TTuple<FName, unsigned char*> item : behaviourBaseTable->GetRowMap())
	{
		tablerowref.Add(reinterpret_cast<FAIPlayerBehaviourTable*>(item.Value));
	}

	// Ich weiß, das ist scheiße, ich bin grade aber ein wenig ausgeburned

	TMap<EAIBehaviourIdentifier, FAIBehaviourBase> allavailablebehaviours = tablerowref[0]->GetBehaviourBase();

	int rnd = FMath::RandRange(1, allavailablebehaviours.Num() - 1);

	currentBehaviourBase = allavailablebehaviours.FindRef((EAIBehaviourIdentifier)rnd);
}

void AAIPlayer::TickSamples()
{
	states->SampleBuyResources();
	states->SampleBuildSite();
	states->SampleTransportResources();
}