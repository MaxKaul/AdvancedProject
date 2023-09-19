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
