#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "Engine/DataTable.h"
#include "ResourceTableBase.generated.h"

USTRUCT(BlueprintType)
struct FResourceTableBase : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		EResourceIdent ItemIdent;

	FResourceTableBase()
	{

	};
};