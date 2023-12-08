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

	FMarketStallSaveData(TArray<EResourceIdent> _stallResources, int _stallID)
	{
		s_stallResources = _stallResources;
		s_stallID = _stallID;
	}

private:
	UPROPERTY()
		TArray<EResourceIdent> s_stallResources;
	UPROPERTY()
		int s_stallID;

public:
	FORCEINLINE
		TArray<EResourceIdent> GetStallResources_S() { return s_stallResources; }
	FORCEINLINE
		int GetStallID() { return s_stallID; }

	FORCEINLINE
		void AddNewProductionType_S(EResourceIdent _toAdd) { s_stallResources.Add(_toAdd); }
	FORCEINLINE
		void SetStallID(int _stallID) { s_stallID = _stallID; }
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

	FORCEINLINE
		int GetStallID() { return stallID; };

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StallInfo, meta = (AllowPrivateAccess))
		int stallID;
};
