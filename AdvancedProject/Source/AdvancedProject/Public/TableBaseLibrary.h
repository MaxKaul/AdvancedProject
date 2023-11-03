#pragma once

#include "CoreMinimal.h"
#include "MarketManager.h"
#include "Productionsite.h"
#include "Engine/DataTable.h"
#include "TableBaseLibrary.generated.h"


USTRUCT(BlueprintType)
struct FResourceTableBase : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		FIndividualResourceInfo Resource;

	FResourceTableBase()
	{

	};
};

USTRUCT(BlueprintType)
struct FProductionSiteInfoBase : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		FProductionSiteDisplayInfos productionSiteDisplayInfos;

	FProductionSiteInfoBase()
	{

	};
};