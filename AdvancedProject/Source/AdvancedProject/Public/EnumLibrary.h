#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EResourceIdent : uint8
{
	ERI_DEFAULT,
	ERI_Gold,
	ERI_Iron,
	ERI_Wheat,
	ERI_Meat,
	ERI_Fruit,
	ERI_Resin,
	ERI_Hardwood,
	ERI_Jewelry,
	ERI_Furniture,
	ERI_Ambrosia,
	ERI_ENTRY_AMOUNT
};

UENUM(BlueprintType)
enum class EProductionSiteType : uint8
{
	PST_DEFAULT,
	PST_Meat,
	PST_Fruits,
	PST_Wheat,
	PST_Hardwood_Resin,
	PST_Gold_Iron,
	PST_Furniture_Jewelry,
	PST_Ambrosia,
	PST_ENTRY_AMOUNT
};

UENUM(BlueprintType)
enum class EWorkerStatus : uint8
{
	WS_DEFAULT,
	WS_Unemployed,
	WS_Unassigned,
	WS_Employed_MainJob,
	WS_Employed_SideJob,
	WS_ENTRY_AMOUNT
};