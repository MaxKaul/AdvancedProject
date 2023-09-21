// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarketManager.h"
#include "GameFramework/Actor.h"
#include "SaveGameManager.generated.h"

UCLASS()
class ADVANCEDPROJECT_API ASaveGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASaveGameManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	bool InitAllManager();

	UFUNCTION()
		bool NullcheckDependencies();

	UFUNCTION()
		bool SaveGameData();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ManagerClasses, meta = (AllowPrivateAccess))
		TSubclassOf<class AMarketManager> marketManagerClass;

	UPROPERTY()
		UWorld* world;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ManagerSpawns, meta = (AllowPrivateAccess))
		AMarketManager* spawnedMarketManager;

	TOptional<FMarketManagerSaveData> marketManagerSaveData;
};
