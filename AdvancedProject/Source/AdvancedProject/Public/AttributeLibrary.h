
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

	FWorkerDesireBase(TArray<EResourceIdent> _prefferedLuxuryResources, float _dmLuxury, float _dmHunger)
	{
		prefferedLuxuryResources = _prefferedLuxuryResources;
		modifier_desireLuxury = _dmLuxury;
		modifier_desireHunger = _dmHunger;
	}

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float modifier_desireLuxury;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float modifier_desireHunger;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		TArray<EResourceIdent> prefferedLuxuryResources;

public:
	FORCEINLINE
		float GetDesireMod_Luxury() { return modifier_desireLuxury; }
	FORCEINLINE
		float GetDesireMod_Hunger() { return modifier_desireHunger; }
	FORCEINLINE
		TArray<EResourceIdent> GetDesired_LuxuryResources() { return prefferedLuxuryResources; }
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