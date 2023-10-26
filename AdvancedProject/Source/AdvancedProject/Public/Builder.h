// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "Productionsite.h"
#include "Components/ActorComponent.h"
#include "Builder.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVANCEDPROJECT_API UBuilder : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuilder();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	bool InitBuilder(class UProductionSiteManager* _manager, class AMarketManager* marketManager);

	// Gespeicherte Productionsites werden aus dem ProductionSiteManager (Welcher unique für alle player entities ist) gespawned
	// Bedeutet der builder wird zum bauen der sites nur gecalled wenn eine player entetie auch tatsächlich etwas baut
	UFUNCTION()
		bool BuildProductionSite(UStaticMesh* _siteMesh, EProductionSiteType _type, ABuildingSite* _buildingSite, class AMarketManager* _marketManager, int _siteID);

private:
	UFUNCTION()
		bool NullcheckDependencies();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dependencies, meta = (AllowPrivateAccess))
		 UProductionSiteManager* productionSiteManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ProductionSite, meta = (AllowPrivateAccess))
		TSubclassOf<class AProductionsite> productionSiteClass;

	UPROPERTY()
		AMarketManager* marketManager;

	UPROPERTY()
		UWorld* world;
};
