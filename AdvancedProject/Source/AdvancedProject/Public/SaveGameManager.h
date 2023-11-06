// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarketManager.h"
#include "PlayerBase.h"
#include "ProductionSiteManager.h"
#include "WorkerWorldManager.h"
#include "GameFramework/Actor.h"
#include "SaveGameManager.generated.h"

USTRUCT()
struct FSaveManagerOptionals
{
	GENERATED_BODY()

		TOptional<FMarketManagerSaveData> marketManagerSaveData;
		TOptional<FWorkerWorldManagerSaveData> workerWorldManagerSaveData;

		TOptional<TArray<FPlayerSaveData>> allPlayerSaveData;
};

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

	UFUNCTION(BlueprintCallable)
		bool SaveGameData();
	UFUNCTION()
		bool LoadGameData(FString _saveGameName, int _saveGameSlot);

	UFUNCTION()
		void SetSaveGameName(FString _name);

private:
	bool InitAllManager();

	UFUNCTION()
		bool NullcheckDependencies();

	FORCEINLINE
		TArray<FString> GetSavedGamesByName() { return savedGames; }

public:
	UFUNCTION()
	void SubscribePlayer(class APlayerBase* _toSub);
	UFUNCTION()
	void UnsubscribePlayer(APlayerBase* _toUnsub);

private:
	UPROPERTY()
		UWorld* world;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ManagerClasses, meta = (AllowPrivateAccess))
		TSubclassOf<AMarketManager> marketManagerClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ManagerSpawns, meta = (AllowPrivateAccess))
		AMarketManager* spawnedMarketManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ManagerClasses, meta = (AllowPrivateAccess))
		TSubclassOf<AWorkerWorldManager> workerWorldManagerClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ManagerSpawns, meta = (AllowPrivateAccess))
		AWorkerWorldManager* spawnedWorkerWorldManager;

	UPROPERTY()
		FSaveManagerOptionals saveManagerOptionals;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SaveGameInfo, meta = (AllowPrivateAccess))
		FString saveGameName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ManagerSpawns, meta = (AllowPrivateAccess))
		TArray<FString> savedGames;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess))
		TArray<class APlayerBase*> allPlayer;
};
