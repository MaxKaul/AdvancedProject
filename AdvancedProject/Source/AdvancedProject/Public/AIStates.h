// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "MarketManager.h"
#include "StateStatusTicketLibrary.h"
#include "Components/ActorComponent.h"
#include "AIStates.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVANCEDPROJECT_API UAIStates : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAIStates();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
		class AAIPlayer* stateOwner;

	UPROPERTY()
		UWorld* world;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void InitState(AAIPlayer* _owner);

	UFUNCTION()
		void State_Wait();
	UFUNCTION()
		bool State_BuyResources();
	UFUNCTION()
		bool State_SellResources();
	UFUNCTION()
		bool State_TransportResources();
	UFUNCTION()
		bool State_BuildSite();
	UFUNCTION()
		bool State_FireWorker();
	UFUNCTION()
		bool State_HireWorker();
	UFUNCTION()
		bool State_AssignWorker();
	UFUNCTION()
		bool State_UnassignWorker();


	UFUNCTION()
		void SampleBuyResources();
	UFUNCTION()
		FStateStatusTicket_BuyResources CalculateBuyOrder();

	UFUNCTION()
		void SampleBuildSite();
	// Ich sollte bei zeiten vill mal schauen warum ich dort nicht die sampleresults zurück geben wollte
	UFUNCTION()
		TMap<EProductionSiteType, class ABuildingSite*> ChooseSiteTypePair();

	
	UFUNCTION()
		void SampleTransportResources();
	UFUNCTION()
		FStateStatusTicket_TransportResources ChooseTransportationTarget();
};