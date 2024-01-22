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
};
