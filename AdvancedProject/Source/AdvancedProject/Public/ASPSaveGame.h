// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarketManager.h"
#include "GameFramework/SaveGame.h"
#include "ASPSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDPROJECT_API UASPSaveGame : public USaveGame
{
	GENERATED_BODY()

		UASPSaveGame();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveGameInfo, meta = (AllowPrivateAccess))
		FMarketManagerSaveData marketManagerSaveData;
public:
	FORCEINLINE
		FMarketManagerSaveData GetSavedMarketManagerData() { return marketManagerSaveData; }

	UFUNCTION()
		void InitSaveGame(FMarketManagerSaveData _marketManagerSaveData);
};
