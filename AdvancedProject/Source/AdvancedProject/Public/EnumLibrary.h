#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EResourceIdent : uint8
{
	ERI_DEFAULT,
	ERI_Gold,
	ERI_Iron,
	ERI_Wheat,
	ERI_Meat,
	ERI_Fruit,
	ERI_Resin,
	ERI_Jewelry,
	ERI_Furniture,
	ERI_Ambrosia,
	ERI_ENTRY_AMOUNT
};

UENUM()
enum class EProductionSiteType : uint8
{
	PST_DEFAULT,
	PST_Meat,
	PST_Fruits,
	PST_Hardwood_Resin,
	PST_Furniture_Jewelry,
	PST_Ambrosia,
	PST_ENTRY_AMOUNT
};

UENUM()
enum class ECameraZoomStatus : uint8
{
	ETS_DEFAULT,
	ETS_ZoomIn,
	ETS_ZoomOut,
	ETS_NoZoom,
	ETS_ENTRY_AMOUNT
};