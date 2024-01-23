// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeLibrary.h"
#include "PlayerBase.h"
#include "AIPlayer.generated.h"


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
		float decisionBaseValue;

	UPROPERTY()
		UWorld* world;

	// Wait sollte immer die höchste chance haben, die KI soll nicht konsatnt entscheidungen treffen
	UFUNCTION()
		void TickDecision();
	UFUNCTION()
		void SetDecisionTimer();

	UFUNCTION()
		void SetNewState(EAIStates _newState);

	UFUNCTION()
		void GenerateAIBiases();
};