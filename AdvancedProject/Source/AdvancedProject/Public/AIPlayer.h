// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeLibrary.h"
#include "PlayerBase.h"
#include "AIPlayer.generated.h"

USTRUCT()
struct FPossibleBuyOrders
{
	GENERATED_BODY()

		FPossibleBuyOrders(){}

private:
	TArray<FResourceTransactionTicket> tickets;

public:
	FORCEINLINE
		TArray<FResourceTransactionTicket> GetTickets() { return tickets; }

	// Jetzt nicht perfekt, aber muss erstmal reichen
	FORCEINLINE
		void AddNewTicket(FResourceTransactionTicket _newTicket) { tickets.Add(_newTicket); }
};

USTRUCT()
struct FStateStatusTicket_BuyResources
{
	GENERATED_BODY()

	FStateStatusTicket_BuyResources(){}

	FStateStatusTicket_BuyResources(bool _isSampleValid, TArray<FResourceTransactionTicket> _transactionTicket)
	{
		transactionTickets = _transactionTicket;
		bIsSampleValid = _isSampleValid;
	}

private:
	UPROPERTY()
		TArray<FResourceTransactionTicket> transactionTickets;

	UPROPERTY()
		bool bIsSampleValid;

public:
	FORCEINLINE
		TArray<FResourceTransactionTicket> GetTransactionTickets() { return transactionTickets; }

	FORCEINLINE
		bool GetValidity() { return bIsSampleValid; }
};



// Ich will mal schaeun das ich was die ai player angeht das so mache das die hier, in AAIPlayer errechnen sollen werlchen state sie erreichen sollen und in EAIStates diese ausführen
// bin mir grad nicht sicher obs andersherum nicht besser wäre, aber ich bin die letzten tage ein wenig ausgebrandt also mach ich das ertstmal und reevaluiere das später, muss das nur im hinterkopf behalten
UCLASS()
class ADVANCEDPROJECT_API AAIPlayer : public APlayerBase
{
	GENERATED_BODY()

public:
	AAIPlayer();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Macht grade Init ohne save, muss noch umgebaut werden, soll halt erstmal alles funzen
	UFUNCTION()
		void IntiAIPlayer();

	FORCEINLINE
		EAIStates GetCurrentState() { return currentState; }
	FORCEINLINE
		EAIStates GetPreviousState() { return previousState; }

protected:
	// Nicht perfekt, ersparrt mir aber jede menge getter und shit
	friend class UAIStates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = States, meta = (AllowPrivateAccess))
		class UAIStates* states;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Behaviour, meta = (AllowPrivateAccess))
		UDataTable* behaviourBaseTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behaviour, meta = (AllowPrivateAccess))
		FAIBehaviourBase currentBehaviourBase;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AIStates, meta = (AllowPrivateAccess))
		EAIStates currentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AIStates, meta = (AllowPrivateAccess))
		EAIStates previousState;

	// This Variable holds the value associated with each possible state that will be compared to the threshhold to progress to another state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AIStates, meta = (AllowPrivateAccess))
		TMap<EAIStates, float> stateProbabilityPair;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AIStates, meta = (AllowPrivateAccess))
		TMap<EAIStates, float> validStatesToTick;

	// The calculation will add up for each DecisionTick, should a value of one stateProbabilityPair exceed the decisionThreshold, the state will be executed, all other propabilities will reset and
	// Once done, the state will be reset to the Wait_State
	// If two or more states exceed the threshold, one will be choosen at random
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIInfo, meta = (AllowPrivateAccess))
		float decisionThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIInfo, meta = (AllowPrivateAccess))
		float decicionTickRateMin;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIInfo, meta = (AllowPrivateAccess))
		float decicionTickRateMax;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIInfo, meta = (AllowPrivateAccess))
		float decisionBaseValueMin;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIInfo, meta = (AllowPrivateAccess))
		float decisionBaseValueMax;

	UPROPERTY()
		UWorld* world;


	// Was ich noch machen könnte wäre mir ein weiteres layer anlegen damit ich die alle in einem struct habne kann
	UPROPERTY()
		FStateStatusTicket_BuyResources sampleResult_BuyResources;
	UPROPERTY()
		TMap<EProductionSiteType, class ABuildingSite*> sampleResult_BuildSite;

	UPROPERTY()
		FTimerHandle buildCooldownHandle;

	// Ich sollte mal schauen das ich das alles etwas in seperate klassen aufteile

	// Wait sollte immer die höchste chance haben, die KI soll nicht konsatnt entscheidungen treffen
	UFUNCTION()
		void TickDecision();
	UFUNCTION()
		void SetDecisionTimer();

	UFUNCTION()
		void SetNewState(EAIStates _newState);

	UFUNCTION()
		void GenerateAIBiases();

	UFUNCTION()
		void TickSamples();
};