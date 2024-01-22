// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "AIPlayer.generated.h"

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

	UFUNCTION()
		void State_Wait();
	UFUNCTION()
		void State_BuyResources();
	UFUNCTION()
		void State_SellResources();
	UFUNCTION()
		void State_TransportResources();
	UFUNCTION()
		void State_BuildSite();
	UFUNCTION()
		void State_FireWorker();
	UFUNCTION()
		void State_HireWorker();
	UFUNCTION()
		void State_AssignWorker();
	UFUNCTION()
		void State_UnassignWorker();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = States, meta = (AllowPrivateAccess))
		class UAIStates* states;
};
