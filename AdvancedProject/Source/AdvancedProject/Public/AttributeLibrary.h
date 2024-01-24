
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

// This struct should store the weight biases the ai player will have for certain actions
USTRUCT(BlueprintType)
struct FAIBehaviourBase
{
	GENERATED_BODY()

		FAIBehaviourBase() {}

private:
	// Pref Tabel
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		bool bHasStatePreference;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		bool bHasSitePreference;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		bool bHasBuyPreference;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		bool bHasSellPreference;

	// Preference weight pair
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), meta = (EditCondition = "bHasStatePreference"))
		TMap<EAIStates, float> statePreference;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), meta = (EditCondition = "bHasSitePreference"))
		TMap<EProductionSiteType, float> sitePreference;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), meta = (EditCondition = "bHasBuyPreference"))
		TMap<EResourceIdent, float> buyPreference;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), meta = (EditCondition = "bHasSellPreference"))
		TMap<EResourceIdent, float> sellPreference;

	// Other stuff

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float productivityGoal;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		int resourceAmountGoal;
	// Dividing value to determine what the highest expenditure per buy action in relation to the total amount of currency will be
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float maxExpenditureDivider;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float siteConstructionCooldown;

public:
	// Mal Schauen ob ich das überhaupt brauche
	// Ich mein is zwar so schon cursed, aber die alternativen für ein weiteres obsurity layer sind auch nicht viel hübscher
	// benui.ca/unreal/uproperty-edit-condition-can-edit-change/
	// thomassampson.co.uk/posts/ue4-uproperty-editcondition/
	TArray<EPreferenceType> GetPreferences()
	{
		TArray<EPreferenceType> prefs;

		if (bHasStatePreference)
			prefs.Add(EPreferenceType::PT_HasStatePreference);
		else if (bHasSitePreference)
			prefs.Add(EPreferenceType::PT_HasSitePreference);
		else if (bHasBuyPreference)
			prefs.Add(EPreferenceType::PT_HasBuyPreference);
		else if (bHasSellPreference)
			prefs.Add(EPreferenceType::PT_HasSellPreference);

		return prefs;
	}

	FORCEINLINE
		TMap<EAIStates, float> GetStatePreferences() { return statePreference; }
	FORCEINLINE
		TMap<EProductionSiteType, float> GetSitePreferences() { return sitePreference; }
	FORCEINLINE
		TMap<EResourceIdent, float>GetBuyPreferences() { return buyPreference; }
	FORCEINLINE
		TMap<EResourceIdent, float> GetSellPreferences() { return sellPreference; }


	FORCEINLINE
		float GetProductivityGoal() { return productivityGoal; }
	FORCEINLINE
		int GetResourceAmountGoal() { return resourceAmountGoal; }
	FORCEINLINE
		float GetExpenditureDivider() { return maxExpenditureDivider; }
	FORCEINLINE
		float GetSiteConstructionCooldown() { return siteConstructionCooldown; }

};


USTRUCT()
struct FAttributeEffetcTableRow : public FTableRowBase
{
	GENERATED_BODY()

		FAttributeEffetcTableRow() {}

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		TMap<EWorkerAttributeIdentifier, FWorkerDesireBase> attributeBase;

public:
	TMap<EWorkerAttributeIdentifier, FWorkerDesireBase> GetAttributeBase() { return attributeBase; }
};



USTRUCT()
struct FAIPlayerBehaviourTable : public FTableRowBase
{
	GENERATED_BODY()
		FAIPlayerBehaviourTable() {}

private:
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess))
		TMap<EAIBehaviourIdentifier, FAIBehaviourBase> behaviourBase;

public:
	TMap<EAIBehaviourIdentifier, FAIBehaviourBase> GetBehaviourBase() { return behaviourBase; }
};
