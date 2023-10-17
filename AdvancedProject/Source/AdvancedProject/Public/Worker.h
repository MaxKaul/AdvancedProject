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
	// Sollte in einer range von 0.005f bis 0.08f eingestellt werden (ERSTMAL), dieser wert wird nähmlich von der ResourceTick rate ABGEZOGEN
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		float productivity;

public:
	FORCEINLINE
		float GetProductivity() { return productivity; }
};
