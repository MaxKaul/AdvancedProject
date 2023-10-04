// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Worker.generated.h"

UCLASS()
class ADVANCEDPROJECT_API AWorker : public ACharacter
{
	GENERATED_BODY()

public:
	AWorker();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		float economicOutput;

public:
	FORCEINLINE
		float GetEconomicOutput() { return economicOutput; }
};
