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

UENUM(BlueprintType)
enum class EWorkerButtonInitStatus : uint8
{
	WS_DEFAULT,
	WS_ForDirectHire,
	WS_ForWorld,
	WS_ForHired,
	WS_ForAssigned,
	WS_ForAllAssigned,
	WS_ENTRY_AMOUNT
};

UENUM(BlueprintType)
enum class EPlayerIdent : uint8
{
	PI_DEFAULT,
	PI_Player_1,
	PI_Player_2,
	PI_Player_3,
	PI_Player_4,
	PI_ENTRY_AMOUNT
};