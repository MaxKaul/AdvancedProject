// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumLibrary.h"
#include "Components/SphereComponent.h"
#include "BuildingSite.generated.h"

UCLASS()
class ADVANCEDPROJECT_API ABuildingSite : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingSite();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void SetBuildStatus(bool _status);

	FORCEINLINE
		bool GetBuildStatus() { return bHasBeenBuildOn; }

	FORCEINLINE
		FVector GetInteractionPos() { return interactionPoint->GetComponentLocation(); }

	FORCEINLINE
		TArray<EProductionSiteType> GetAllowedTypes() { return allowedTypes; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Side Infos", meta=(AllowPrivateAccess))
		TArray<EProductionSiteType> allowedTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess))
		UStaticMeshComponent* actorMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess))
		UMaterialInterface* translucentMat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess))
		class USphereComponent* interactionPoint;

	UPROPERTY()
		bool bHasBeenBuildOn;
};
