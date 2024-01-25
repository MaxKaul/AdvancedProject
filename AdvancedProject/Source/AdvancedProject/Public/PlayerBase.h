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

	void InitPlayerSaveData(FVector _location, FRotator _rotation, FProductionSiteManagerSaveData _siteManagerSaveData, EPlayerIdent _ident, FTransportManagerSaveData _transportManagerSaveData, float _currency)
	{
		s_location = _location;
		s_rotation = _rotation;
		s_productionSiteManagerSaveData = _siteManagerSaveData;
		s_playerIdent = _ident;
		s_transportManagerSaveData = _transportManagerSaveData;
		s_currency = _currency;
	}

private:
	UPROPERTY()
		FVector s_location;
	UPROPERTY()
		FRotator s_rotation;
	UPROPERTY()
		FProductionSiteManagerSaveData s_productionSiteManagerSaveData;
	UPROPERTY()
		EPlayerIdent s_playerIdent;
	UPROPERTY()
		FTransportManagerSaveData 	s_transportManagerSaveData;
	UPROPERTY()
		float s_currency;

public:
	FORCEINLINE
		FVector GetLoaction_S() { return s_location; }
	FORCEINLINE
		EPlayerIdent GetIdent_S() { return s_playerIdent; }
	FORCEINLINE
		FRotator GetRotation_S() { return s_rotation; }
	FORCEINLINE
		FProductionSiteManagerSaveData GetProductionSiteManagerSaveData_S() { return s_productionSiteManagerSaveData; }
	FORCEINLINE
		FTransportManagerSaveData GeTransportManagerSaveData_S() { return s_transportManagerSaveData; }
	FORCEINLINE
		float GetCurrency_S() { return s_currency; }
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
		virtual void InitPlayerStart(FPlayerSaveData _saveData, AMarketManager* _marketManager, class AWorkerWorldManager* workerWorldManager, TArray<ABuildingSite*> _allBuildingSites);
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

	UPROPERTY()
		TArray<ABuildingSite*> allBuildingSites;

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