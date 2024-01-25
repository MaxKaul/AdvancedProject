// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarketManager.h"
#include "Components/ActorComponent.h"
#include "TransportManager.generated.h"

USTRUCT(BlueprintType)
struct FTransportOrder
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hash)
		FString structName_Saved = FString("NONE");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hash)
		FString productionSiteID_Saved = FString("NONE");

public:
	FORCEINLINE
		bool operator==(const  FTransportOrder& _other) const
	{
		return Equals(_other);
	}

	FORCEINLINE
		bool operator!=(const  FTransportOrder& _other) const
	{
		return !Equals(_other);
	}

	FORCEINLINE
		bool Equals(const  FTransportOrder& _other) const
	{
		return structName_Saved == _other.structName_Saved && productionSiteID_Saved == _other.productionSiteID_Saved;
	}

	FTransportOrder(){}


	FTransportOrder(TArray<FResourceTransactionTicket> _transactionOrder, float _timeRemaining, TMap<EBuildingType, int> _goalTypeIDPair, ETransportOrderStatus _transportOrderStatus, int _owningSiteID,
					ETransportatOrderDirecrtive _transportDirective , UWorld* _world)
	{
		transactionOrder = _transactionOrder;
		timeRemaining = _timeRemaining;
		transportOrderStatus = _transportOrderStatus;
		goalTypeIDPair = _goalTypeIDPair;
		owningSiteID = _owningSiteID;
		transportDirective = _transportDirective;
		world = _world;
	}

private:
	UPROPERTY()
		TArray<FResourceTransactionTicket> transactionOrder;

	UPROPERTY()
		float timeRemaining;

	// Ich "weiß" ob mein goalactor der markt oder eine production site ist über den transportOrderStatus (nicht perfekt aber so muss ich die ganze buy und sell logik nicht refaktorieren)
	UPROPERTY()
		TMap<EBuildingType, int> goalTypeIDPair;

	UPROPERTY()
		ETransportOrderStatus transportOrderStatus;

	UPROPERTY()
		int owningSiteID;

	UPROPERTY()
		ETransportatOrderDirecrtive transportDirective;

	UPROPERTY()
		UWorld* world;

public:
	FORCEINLINE
		TArray<FResourceTransactionTicket> GetTransactionOrder() { return transactionOrder; };
	FORCEINLINE
		float GetTimeRemaining() { return timeRemaining; }
	FORCEINLINE
		TMap<EBuildingType, int> GetGoalActorTypeIDPair() { return goalTypeIDPair; }
	FORCEINLINE
		ETransportOrderStatus GetTransportOrderStatus() { return transportOrderStatus; }
	FORCEINLINE
		int GetOwningProductionSiteID() { return owningSiteID; }
	FORCEINLINE
		ETransportatOrderDirecrtive GetOrderDirective() { return transportDirective; }
};

// Ich glaub ich muss das um ein layer weiter nach oben legen da mir die refs zu den ownern und goals sonst zu converluded sind
USTRUCT()
struct FTransportManagerSaveData
{
	GENERATED_BODY()

	FTransportManagerSaveData(){}

	FTransportManagerSaveData(TArray<FTransportOrder> _allransportOrders)
	{
		s_allTransportOrders = _allransportOrders;
	}

private:
	UPROPERTY()
		TArray<FTransportOrder> s_allTransportOrders;

public:
	FORCEINLINE
		TArray<FTransportOrder> GetAllTransportOrders_S() { return s_allTransportOrders; }
};

FORCEINLINE uint32 GetTypeHash(const  FTransportOrder& _this)
{
	const uint32 Hash = FCrc::MemCrc32(&_this, sizeof(FTransportOrder));
	return Hash;
}

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVANCEDPROJECT_API UTransportManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTransportManager();

	UFUNCTION()
		FTransportManagerSaveData GetTransportManagerSaveData();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(VisibleAnywhere,Category=Transporter,meta=(AllowPrivateAccess))
		TMap<FTransportOrder, FTimerHandle> allTransportOrders;
	//UPROPERTY(VisibleAnywhere, Category = Transporter, meta = (AllowPrivateAccess))
	//	TArray<FTimerHandle> fuckthisshit;

	UPROPERTY()
		class UProductionSiteManager* productionSiteManager;

	UPROPERTY()
		UWorld* world;

	UPROPERTY()
		AMarketManager* marketManager;
	UPROPERTY()
		class APlayerBase* owningPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
		float transportSpeed;

public:
	UFUNCTION()
	void InitTransportManager(APlayerBase* _owningPlayer, FTransportManagerSaveData _saveData, AMarketManager* _marketManager, UProductionSiteManager* _prodSiteManager);

	UFUNCTION(BlueprintCallable)
		void TestOrder();
	UFUNCTION(BlueprintCallable)
		void TestOrder_0();

private:
	UFUNCTION()
		void ManageTransaction(FTransportOrder _orderToHandle);
	UFUNCTION()
		void HandleMarketTransaction(FTransportOrder _orderToHandle);
	UFUNCTION()
		void HandleProdSiteTransaction(FTransportOrder _orderToHandle);

	// Ich schau mal das ich den goal actor dann auf die erste prod site defaulte, das SOLLTE eigentliche als debugger fungieren falls was schief läuft beim loaden da dies die tarnsportation order zurücksetzt
	// (hoff ich mal (Ich hoff aber auch mal das nichts schief läuft))
	// Ich könnte auch malo drüber nachdenken dies auf den origin zu defaulten
	UFUNCTION()
		AActor* GetGoalActor(TMap<EBuildingType, int> _goalTypeIDPair);

	// Ich nehmen den goal actor raus und mache diesen dann komplett abhängig von der tranportation directive
	// Ich mach das jetzt etwas dreckig weil ich mit dem overload struct probleme habe persistenz für den owner sicher zu stellen
	// _overridePos = Wenn ich ein Order mit TOD_ReturnToSite erstellen will brauch ich meine momentane pos (mein erstes goal) für die path länge
	UFUNCTION()
		void CreateTransportOrder(TArray<FResourceTransactionTicket> _transaction, ETransportOrderStatus _orderStatus, class AProductionsite* _owningSite, ETransportatOrderDirecrtive _transportDirective, TMap<EBuildingType, int> _goalTypeIDPair, FVector _overridePos = FVector());
	UFUNCTION()
		void LoadOrderFromSave(FTransportManagerSaveData _saveData);

	// Ich will die menge an resourcen in meinem owner pool samplen wenn ich eine deduction order von ihr herraus sende und unter dem umstand die menge an resourcen <= 0 ist 0 geben u./o. < gewünschte menge den rest herrausgeben
	UFUNCTION()
		TArray<FResourceTransactionTicket> SampleSitePool(TArray<FResourceTransactionTicket> _ticketsToCheck, class AProductionsite* _siteToSample);
};
