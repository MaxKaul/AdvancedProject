// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "MarketStall.generated.h"

USTRUCT()
struct FMarketStallSaveData
{
	GENERATED_BODY()

	FMarketStallSaveData(){};

	FMarketStallSaveData(TArray<EProductionSiteType> _stallType)
	{
		stallType = _stallType;
	}

private:
	UPROPERTY()
		TArray<EProductionSiteType> stallType;

public:
	FORCEINLINE
		TArray<EProductionSiteType> GetSiteType() { return stallType; }

	FORCEINLINE
		void AddNewProductionType(EProductionSiteType _toAdd) { stallType.Add(_toAdd); }
};

UCLASS()
class ADVANCEDPROJECT_API AMarketStall : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AMarketStall();

	UFUNCTION()
	void InitMarketStall(UStaticMesh* _newMesh, FMarketStallSaveData _saveData);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION() FORCEINLINE
		TArray<EProductionSiteType> GetStallType() { return stallTypes; }

	UFUNCTION()
		FMarketStallSaveData GetStallSaveData();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess))
		UStaticMeshComponent* actorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess))
		UBillboardComponent* editorVisual;

	// Jeder stall kann zwei resourcen kaufen verkaufen
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Infos, meta = (AllowPrivateAccess))
		TArray<EProductionSiteType> stallTypes;
};
