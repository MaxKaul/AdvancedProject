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

	FMarketStallSaveData(TArray<EResourceIdent> _stallResources)
	{
		stallResources = _stallResources;
	}

private:
	UPROPERTY()
		TArray<EResourceIdent> stallResources;

public:
	FORCEINLINE
		TArray<EResourceIdent> GetSiteType() { return stallResources; }

	FORCEINLINE
		void AddNewProductionType(EResourceIdent _toAdd) { stallResources.Add(_toAdd); }
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
		TArray<EResourceIdent> GetStallResources() { return stallResources; }

	UFUNCTION()
		FMarketStallSaveData GetStallSaveData();

	UFUNCTION() FORCEINLINE
		UStaticMeshComponent* GetMesh() { return actorMesh; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess))
		UStaticMeshComponent* actorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess))
		UBillboardComponent* editorVisual;

	// Jeder stall kann zwei resourcen kaufen verkaufen
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Infos, meta = (AllowPrivateAccess))
		TArray<EResourceIdent> stallResources;
};
