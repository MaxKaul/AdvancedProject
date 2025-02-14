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

	FORCEINLINE
		int GetMaxPlayerAmount() { return maxPlayerAmount; }

private:
	UFUNCTION()
		bool InitAllManager();
	UFUNCTION()
		bool InitMarketManager();
	UFUNCTION()
		bool InitPlayer();
	UFUNCTION()
		void InitFreshPlayer(APlayerBase* _newplayer, EPlayerIdent _ident);

	// Der WorkerWorldManager wird mit save data erst gespawned und dann nach dem spawnen aller spieler Init
	// -> Der WorkerWorldManager ben�tigt eine Liste aller productionsites, diese spawnen mit der save data allerdings �ber den spieler und der spieler ben�tigt den
	// WorkerWorldManager, diese braucht allerdings noch nicht initialised sein
	// -> Dies muss noch mit mehreren spielern getestet werde
	UFUNCTION()
		bool SpawnWorkerWorldManager();
	UFUNCTION()
		bool InitWorkerWorldManager();

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = World, meta = (AllowPrivateAccess))
		TArray<class ABuildingSite*> allBuildingSites;

	UPROPERTY(VisibleAnywhere)
		TArray<AProductionsite*> allSavedProductionsites;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess))
		TSubclassOf<APlayerBase> aiPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess))
		TSubclassOf<class ASpectatorPlayer> spectatorPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess))
		TArray<class APlayerBase*> allPlayer;

	// Max Player count should alwas be <= 4
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess))
		int maxPlayerAmount;

	// Set true to deactivate player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess))
		bool bPlayAsSimulated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess))
		FVector defaultStartPos;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess))
		FRotator defaultStartRot;
};
