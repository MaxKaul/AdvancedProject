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
	WS_Assigned_SideJob,

	WS_FullfillNeed,
	WS_FinishFullfillNeed,

	WS_Unassigned,
	WS_Assigned_MainJob,
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
	PI_World,
	PI_ENTRY_AMOUNT
};

UENUM(BlueprintType)
enum class EWorkerAttributeIdentifier : uint8
{
	WAI_DEFAULT,
	WAI_A1,
	WAI_A2,
	WAI_A3,
	WAI_A4,
	WAI_ENTRY_AMOUNT
};

UENUM(BlueprintType)
enum class ETransportOrderStatus : uint8
{
	TOS_DEFAULT,
	TOS_MoveToMarket,
	TOS_MoveToProdSite,
	TOS_ENTRY_AMOUNT
};

/*
 * Notizen zu dem TransportManager Problem vom 11.01.24 in Git ( Nur falls ich das vergesse )
 */

UENUM(BlueprintType)
enum class ETransportatOrderDirecrtive : uint8
{
	TOD_DEFAULT,
	TOD_SellResources,
	TOD_BuyResources,
	TOD_DeliverToSite,
	TOD_FetchFromSite,
	TOD_ReturnToSite,
	TOD_ENTRY_AMOUNT
};

UENUM(BlueprintType)
enum class EBuildingType : uint8
{
	BT_DEFAULT,
	BT_ProductionSite,
	BT_MarketStall,
	BT_ENTRY_AMOUNT 
};

UENUM(BlueprintType)
enum class EAIBehaviourIdentifier : uint8
{
	AIBI_DEFAULT,
	AIBI_A1,
	AIBI_A2,
	AIBI_A3,
	AIBI_A4,
	AIBI_ENTRY_AMOUNT,
};

UENUM(BlueprintType)
enum class EAIStates : uint8
{
	AIS_DEFAULT,
	AIS_Wait,
	AIS_BuyResources,
	AIS_SellResources,
	AIS_TransportResources,
	AIS_BuildSite,
	AIS_FireWorker,
	AIS_HireWorker,
	AIS_AssignWorker,
	AIS_UnassignWorker,
	AIS_ENTRY_AMOUNT,
};

UENUM(BlueprintType)
enum class EPreferenceType : uint8
{
	PT_DEFAULT,
	PT_HasStatePreference,
	PT_HasSitePreference,
	PT_HasBuyPreference,
	PT_HasSellPreference,
	PT_ENTRY_AMOUNT
};