// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "MarketStall.generated.h"

UCLASS()
class ADVANCEDPROJECT_API AMarketStall : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AMarketStall();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void SetMesh(UStaticMesh*_newMesh);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess))
		UStaticMeshComponent* actorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess))
		UBillboardComponent* editorVisual;
};
