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
	
	validStatesToTick = stateProbabilityPair;

	TickSamples();

	GenerateAIBiases();
	SetDecisionTimer();
}

void AAIPlayer::TickDecision()
{
	bool bcanresetself = true;

	// Etwas harcode aber fuck it, drecks woche
	// Alles AIDS hier
	// Der wirft hier nen error, kˆnnte sein das das wegen meiner runtime manipulation ist, aber l‰sst das programm jetzt auch nicht abschmieren


	for (int i = (int)validStatesToTick.begin().Key(); i != (int)validStatesToTick.end().Key(); i++)
	{
		EPossibleAIStates stateident = (EPossibleAIStates)i;
		float statevalue = validStatesToTick.FindRef(stateident);
		

		TickSamples();

		//if (stateident != EPossibleAIStates::PAIS_BuyResources)
		//	continue;


		float newvalue = statevalue;
		newvalue += FMath::RandRange(decisionBaseValueMin, decisionBaseValueMax);
		float prefvalue = 1.f;


		if (float value = currentBehaviourBase.GetStatePreferences().Contains(stateident))
			prefvalue *= value;

		stateProbabilityPair.Add(stateident, newvalue);

		if (statevalue >= decisionThreshold)
		{
			SetNewState(stateident);
			bcanresetself = false;
			break;
		}
	}

	//for (TTuple<EPossibleAIStates, float> updatestate : validStatesToTick)
	//{
	//	TickSamples();
	//
	//	if(updatestate.Key != EPossibleAIStates::PAIS_BuyResources)
	//		continue;
	//
	//
	//	float newvalue = updatestate.Value;
	//	newvalue += FMath::RandRange(decisionBaseValueMin , decisionBaseValueMax);
	//	float prefvalue = 1.f;
	//
	//
	//	if (float value = currentBehaviourBase.GetStatePreferences().Contains(updatestate.Key))
	//		prefvalue *= value;
	//
	//	stateProbabilityPair.Add(updatestate.Key, newvalue);
	//
	//	if(updatestate.Value >= decisionThreshold)
	//	{
	//		SetNewState(updatestate.Key);
	//		bcanresetself = false;
	//		break;
	//	}
	//}

	if(bcanresetself)
		SetDecisionTimer();
}

// Muss ich vill nochmal ‰ndern, der wird jetzt zwie mal gecalled wenn der state gewechselt wird, einmal vom statechange wenn if(currstate.Value >= decisionThreshold)
// Und einmal vom StateChange in der switch, da jeder state damit endet das die ki wieder in den WaitingState zur¸ck geht
void AAIPlayer::SetDecisionTimer()
{
	if (world->GetTimerManager().GetTimerRemaining(tickHandle) > 0)
		return;

	// Keine ahnung warum, aber SetTimer klappt hier irgendwie net, wenn ich versuche eine UFunc zu binden
	auto ticklambda = [this]()
	{
		TickDecision();
	};

	FTimerDelegate  currdelegate;
	currdelegate.BindLambda(ticklambda);

	FTimerHandle handle;

	float time = FMath::RandRange(decicionTickRateMin, decicionTickRateMax);


	world->GetTimerManager().SetTimer(tickHandle, currdelegate, time, false);
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

	// Ich weiﬂ, das ist scheiﬂe, ich bin grade aber ein wenig ausgeburned

	TMap<EAIBehaviourIdentifier, FAIBehaviourBase> allavailablebehaviours = tablerowref[0]->GetBehaviourBase();

	int rnd = FMath::RandRange(1, allavailablebehaviours.Num() - 1);

	currentBehaviourBase = allavailablebehaviours.FindRef((EAIBehaviourIdentifier)rnd);
}

void AAIPlayer::TickSamples()
{
	states->SampleBuyResources();
	states->SampleBuildSite();
}