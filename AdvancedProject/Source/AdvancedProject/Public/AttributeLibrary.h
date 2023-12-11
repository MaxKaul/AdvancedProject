
#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "TableBaseLibrary.h"
#include "AttributeLibrary.generated.h"


USTRUCT()
struct FWorkerDesireBase
{
	GENERATED_BODY()

public:
	FWorkerDesireBase() {}

	FWorkerDesireBase(TMap<EResourceIdent, float> _prefferedLuxuryWeightPair, TMap<EResourceIdent, float> _prefferedNutritionWeightPair)
	{
		prefferedLuxuryBiasWeightPairs = _prefferedLuxuryWeightPair;
		prefferedNutritionBiasWeightPairs = _prefferedNutritionWeightPair;
	}

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		TMap<EResourceIdent, float> prefferedLuxuryBiasWeightPairs;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		TMap<EResourceIdent, float> prefferedNutritionBiasWeightPairs;
public:
	FORCEINLINE
		TMap<EResourceIdent, float> GetDesired_LuxuryWeightPairs() { return prefferedLuxuryBiasWeightPairs; }
	FORCEINLINE
		TMap<EResourceIdent, float> GetDesired_NutritionWeightPairs() { return prefferedNutritionBiasWeightPairs; }
};

USTRUCT()
struct FAttributeEffetcTableRow : public FTableRowBase
{
	GENERATED_BODY()

		FAttributeEffetcTableRow() {}

public:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		TMap<EWorkerAttributeIdentifier, FWorkerDesireBase> attributeBase;
};