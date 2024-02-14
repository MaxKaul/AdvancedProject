// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeLibrary.h"
#include "PlayerBase.h"
#include "StateStatusTicketLibrary.h"
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

// Kack name, ich weiß
// Struct zum cachen der Info was und wie viel ich auf welche site transportieren kann
USTRUCT(BlueprintType)
struct FTransportSample_Deliver
{
	GENERATED_BODY()

	FTransportSample_Deliver(){}


	FTransportSample_Deliver(AProductionsite* _site, TMap<EResourceIdent, int> _resourceAmountPair)
	{
		site = _site;
		resourceAmountPair = _resourceAmountPair;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hash)
		FString structName = FString("NONE");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hash)
		FString structID = FString("NONE");

public:
	FORCEINLINE
		bool operator==(const  FTransportSample_Deliver& _other) const
	{
		return Equals(_other);
	}

	FORCEINLINE
		bool operator!=(const  FTransportSample_Deliver& _other) const
	{
		return !Equals(_other);
	}

	FORCEINLINE
		bool Equals(const  FTransportSample_Deliver& _other) const
	{
		return structName == _other.structName && structID == _other.structID;
	}

private:
	UPROPERTY()
		AProductionsite* site;
	UPROPERTY()
		TMap<EResourceIdent, int> resourceAmountPair;

public:
	FORCEINLINE
		AProductionsite* GetSite() { return site; }
	FORCEINLINE
		TMap<EResourceIdent, int> GetResourceIdentAmountPair() { return resourceAmountPair; }
};

FORCEINLINE uint32 GetTypeHash(const  FTransportSample_Deliver& _this)
{
	const uint32 Hash = FCrc::MemCrc32(&_this, sizeof(FTransportSample_Deliver));
	return Hash;
}

// Jede take site ist mit einer struct insatnce associated, diese soll die site besitzen aus welcher genommen werden kann, die resource idents welche aus ihr genommen werden können
// die mit den ident associated menge
USTRUCT(BlueprintType)
struct FSellSample_Check

{
	GENERATED_BODY()

	FSellSample_Check(){}

	// goalsite muss nur != nullptr sein wenn die sell order zu einer prod site transporten soll
	FSellSample_Check(AProductionsite* _takeSite, TMap<EResourceIdent, int> _takeIdentAmountPair, TArray<AProductionsite*> _transportSites = TArray<AProductionsite*>())
	{
		takeSite = _takeSite;
		takeReosurceAmountPair = _takeIdentAmountPair;
		validTransportSites = _transportSites;
	}

private:
	UPROPERTY()
		AProductionsite* takeSite;
	UPROPERTY()
		TMap<EResourceIdent, int> takeReosurceAmountPair;

	UPROPERTY()
		TArray<AProductionsite*> validTransportSites;

	UPROPERTY()
		AProductionsite* transportSite;

public:
	FORCEINLINE
		AProductionsite* GetTakeSite() { return takeSite; }
	FORCEINLINE
		AProductionsite* GetTransportSite() { return transportSite; }
	FORCEINLINE
		TMap<EResourceIdent, int> GetResourceAmountPair() { return takeReosurceAmountPair; }
	FORCEINLINE
		TArray<AProductionsite*> GetValidTransportSites() { return validTransportSites; }

	FORCEINLINE
		void SetTransportSite(AProductionsite* _transportSite) { transportSite = _transportSite; }
};


// Ich will mal schaeun das ich was die ai player angeht das so mache das die hier, in AAIPlayer errechnen sollen werlchen state sie erreichen sollen und in EPossibleAIStates diese ausführen
// bin mir grad nicht sicher obs andersherum nicht besser wäre, aber ich bin die letzten tage ein wenig ausgebrandt also mach ich das ertstmal und reevaluiere das später, muss das nur im hinterkopf behalten
UCLASS()
class ADVANCEDPROJECT_API AAIPlayer : public APlayerBase
{
	GENERATED_BODY()

public:
	AAIPlayer();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void InitPlayerStart(FPlayerSaveData _saveData, AMarketManager* _marketManager, AWorkerWorldManager* _workerWorldManager, TArray<ABuildingSite*> _allBuildingSites) override;

	FORCEINLINE
		EPossibleAIStates GetCurrentState() { return currentState; }
	FORCEINLINE
		EPossibleAIStates GetPreviousState() { return previousState; }

protected:
	// Nicht perfekt, ersparrt mir aber jede menge getter und shit
	friend class UAIStates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = States, meta = (AllowPrivateAccess))
		class UAIStates* states;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Building, meta = (AllowPrivateAccess))
		TArray<UStaticMesh*> allBuildingMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Behaviour, meta = (AllowPrivateAccess))
		UDataTable* behaviourBaseTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behaviour, meta = (AllowPrivateAccess))
		FAIBehaviourBase currentBehaviourBase;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AIStates, meta = (AllowPrivateAccess))
		EPossibleAIStates currentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AIStates, meta = (AllowPrivateAccess))
		EPossibleAIStates previousState;

	// This Variable holds the value associated with each possible state that will be compared to the threshhold to progress to another state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AIStates, meta = (AllowPrivateAccess))
		TMap<EPossibleAIStates, float> stateProbabilityPair;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AIStates, meta = (AllowPrivateAccess))
		TMap<EPossibleAIStates, float> validStatesToTick;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AIStates, meta = (AllowPrivateAccess))
		TMap<EPossibleAIStates, float> currentTickStates;

	// The calculation will add up for each DecisionTick, should a value of one stateProbabilityPair exceed the decisionThreshold, the state will be executed, all other propabilities will reset and
	// Once done, the state will be reset to the Wait_State
	// If two or more states exceed the threshold, one will be choosen at random
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIInfo, meta = (AllowPrivateAccess))
		float decisionThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIInfo, meta = (AllowPrivateAccess))
		float decicionTickRateMin;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIInfo, meta = (AllowPrivateAccess))
		float decicionTickRateMax;

	// Ich kann mal schauen das ich die decision values füer mehrere base values nehme und die dynamic durch die preferences der attributes regel
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIInfo, meta = (AllowPrivateAccess))
		float decisionBaseValueMin;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIInfo, meta = (AllowPrivateAccess))
		float decisionBaseValueMax;


	UPROPERTY()
		UWorld* world;

		FTimerHandle tickHandle;

	// Was ich noch machen könnte wäre mir ein weiteres layer anlegen damit ich die alle in einem struct habne kann
	UPROPERTY()
		FStateStatusTicket_BuyResources sampleResult_BuyResources;
	UPROPERTY()
		FStateStatusTicket_BuildProdSite sampleResult_BuildSite;
	UPROPERTY()
		FStateStatusTicket_TransportResources sampleResult_TransportResources;
	UPROPERTY()
		FStateStatusTicket_SellResources sampleResult_SellResources;

	UPROPERTY()
		TArray<EProductionSiteType> allProdSiteTypes;

	FTimerHandle buildCooldownHandle;

	// Ich sollte mal schauen das ich das alles etwas in seperate klassen aufteile

	// Wait sollte immer die höchste chance haben, die KI soll nicht konsatnt entscheidungen treffen
	UFUNCTION()
		void TickDecision();
	UFUNCTION()
		void SetDecisionTimer();

	UFUNCTION()
		void SetNewState(EPossibleAIStates _newState);

	UFUNCTION()
		void GenerateAIBiases();

	UFUNCTION()
		void TickSamples();
};