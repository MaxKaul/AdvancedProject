#pragma once

#include "CoreMinimal.h"
#include "MarketManager.h"
#include "Engine/DataTable.h"
#include "ResourceTableBase.generated.h"

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