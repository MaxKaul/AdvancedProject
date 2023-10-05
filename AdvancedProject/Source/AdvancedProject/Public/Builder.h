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
	bool InitBuilder(class AProductionSiteManager* _manager, class AMarketManager* marketManager);

	UFUNCTION()
		bool BuildProductionSite(FProductionSiteSaveData _siteData);
private:
	UFUNCTION()
		bool NullcheckDependencies();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dependencies, meta = (AllowPrivateAccess))
		 AProductionSiteManager* productionSiteManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ProductionSite, meta = (AllowPrivateAccess))
		TSubclassOf<class AProductionsite> productionSiteClass;

	UPROPERTY()
		AMarketManager* marketManager;

	UPROPERTY()
		UWorld* world;
};
