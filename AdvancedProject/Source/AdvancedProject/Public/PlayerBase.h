// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProductionSiteManager.h"
#include "GameFramework/Character.h"
#include "PlayerBase.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
	GENERATED_BODY()

	FPlayerSaveData() {}

	void InitSaveData(FVector _location, FRotator _rotation, FProductionSiteManagerSaveData _siteManagerSaveData)
	{
		location = _location;
		rotation = _rotation;
		productionSiteManagerSaveData = _siteManagerSaveData;
	}

private:
	UPROPERTY()
		FVector location;
	UPROPERTY()
		FRotator rotation;
	UPROPERTY()
		FProductionSiteManagerSaveData productionSiteManagerSaveData;

public:
	FORCEINLINE
		FVector GetLoaction() { return location; }
	FORCEINLINE
		FRotator GetRotation() { return rotation; }
	FORCEINLINE
		FProductionSiteManagerSaveData GetProductionSiteManagerSaveData() { return productionSiteManagerSaveData; }
};

UCLASS()
class ADVANCEDPROJECT_API APlayerBase : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerBase();

	UFUNCTION()
		virtual FPlayerSaveData GetPlayerSaveData();
	UFUNCTION()
		virtual void InitPlayer(FPlayerSaveData _saveData, AMarketManager* _marketManager);
	UFUNCTION(BlueprintCallable)
		virtual UProductionSiteManager* GetProductionSiteManager();
	UFUNCTION(BlueprintCallable)
		virtual AWorkerWorldManager* GetWorkerWorldManager();

	// Sollte überarbeitet werden
	// -> Grund hierfür ist -> 
	/*
	 * Das essentielle problem ist das der WorkerWorldManager, sollte ein save vorhanden sein eine liste aller production site benötigt um die worker wieder darauf zu verteilen. 
	 *	Dieses Array wird aber von den Spielern gehalten welche auf grundlage dessen die productionsites spawnen.
	 *	Nun Kann der Spieler nicht vor dem WorkerWorldManager gespawned werden weil ddieser eine dependencie für den player darstellt.
	 */
	UFUNCTION(BlueprintCallable)
		virtual void SetLateInits(AWorkerWorldManager* _workerWorldManager);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
		AWorkerWorldManager* workerWorldManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
		class UBuilder* builder;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
		class AMarketManager* marketManager;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
		class UProductionSiteManager* productionSiteManager;
};