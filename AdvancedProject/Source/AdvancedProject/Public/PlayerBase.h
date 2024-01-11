// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProductionSiteManager.h"
#include "TransportManager.h"
#include "GameFramework/Character.h"
#include "PlayerBase.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
	GENERATED_BODY()

	FPlayerSaveData(){ }

	void InitPlayerSaveData(FVector _location, FRotator _rotation, FProductionSiteManagerSaveData _siteManagerSaveData, EPlayerIdent _ident, FTransportManagerSaveData _transportManagerSaveData)
	{
		location = _location;
		rotation = _rotation;
		productionSiteManagerSaveData = _siteManagerSaveData;
		playerIdent = _ident;
	}

private:
	UPROPERTY()
		FVector location;
	UPROPERTY()
		FRotator rotation;
	UPROPERTY()
		FProductionSiteManagerSaveData productionSiteManagerSaveData;
	UPROPERTY()
		EPlayerIdent playerIdent;
	UPROPERTY()
		FTransportManagerSaveData 	transportManagerSaveData;

public:
	FORCEINLINE
		FVector GetLoaction() { return location; }
	FORCEINLINE
		EPlayerIdent GetIdent() { return playerIdent; }
	FORCEINLINE
		FRotator GetRotation() { return rotation; }
	FORCEINLINE
		FProductionSiteManagerSaveData GetProductionSiteManagerSaveData() { return productionSiteManagerSaveData; }
	FORCEINLINE
		FTransportManagerSaveData GeTransportManagerSaveData() { return transportManagerSaveData; }
};

UCLASS(BlueprintType)
class ADVANCEDPROJECT_API APlayerBase : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerBase();

	UFUNCTION()
		virtual FPlayerSaveData GetPlayerSaveData();
	UFUNCTION()
		virtual void InitPlayerStart(FPlayerSaveData _saveData, AMarketManager* _marketManager, class AWorkerWorldManager* workerWorldManager);
	UFUNCTION(BlueprintCallable)
		virtual UProductionSiteManager* GetProductionSiteManager();
	UFUNCTION(BlueprintCallable)
		virtual AWorkerWorldManager* GetWorkerWorldManager();

	UFUNCTION() FORCEINLINE
		EPlayerIdent GetPlayerIdent() { return playerIdent; };

	UFUNCTION()
		void AddOrDeductCurrency(float _amount);

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		AWorkerWorldManager* workerWorldManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		class UBuilder* builder;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		AMarketManager* marketManager;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		UTransportManager* transportManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = PlayerInfo, meta=(AllowPrivateAccess))
		EPlayerIdent playerIdent;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
		class UProductionSiteManager* productionSiteManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PlayerInfo, meta = (AllowPrivateAccess))
		float payementTickValue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PlayerInfo, meta = (AllowPrivateAccess))
		float playerCurrency;
protected:
	UFUNCTION()
		virtual void TickWorkerPayement();

};