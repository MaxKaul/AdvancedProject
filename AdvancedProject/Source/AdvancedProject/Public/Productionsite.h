// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "GameFramework/Actor.h"
#include "Productionsite.generated.h"

UCLASS()
class ADVANCEDPROJECT_API AProductionsite : public AActor
{
	GENERATED_BODY()
	
public:	
	AProductionsite();

protected:
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
		void InitProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type);

private:
	UFUNCTION()
		bool NullcheckDependencies();

private:
	UPROPERTY()
		UStaticMesh* actorMesh;
	UPROPERTY()
		UStaticMeshComponent* actorMeshComponent;

	UPROPERTY()
		EProductionSiteType productionSiteType;
};
