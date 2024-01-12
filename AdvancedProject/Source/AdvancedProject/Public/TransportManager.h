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


	FTransportOrder(TArray<FResourceTransactionTicket> _transactionOrder, FTimerHandle _timerHandle, AActor* _goalActor, ETransportOrderStatus _transportOrderStatus, class AProductionsite* _owningProdSite,
					ETransportatOrderDirecrtive _transportDirective)
	{
		transactionOrder = _transactionOrder;
		timerHandle = _timerHandle;
		transportOrderStatus = _transportOrderStatus;
		goalActor = _goalActor;
		owningProdSite = _owningProdSite;
		transportDirective = _transportDirective;
	}

private:
	UPROPERTY()
		TArray<FResourceTransactionTicket> transactionOrder;

	// Ich regel das saven der remainging time der transporter über deren individuelle handles 
	UPROPERTY()
		FTimerHandle timerHandle;

	// Ich "weiß" ob mein goalactor der markt oder eine production site ist über den transportOrderStatus (nicht perfekt aber so muss ich die ganze buy und sell logik nicht refaktorieren)
	UPROPERTY()
		AActor* goalActor;

	UPROPERTY()
		ETransportOrderStatus transportOrderStatus;

	UPROPERTY()
		AProductionsite* owningProdSite;
	UPROPERTY()
		ETransportatOrderDirecrtive transportDirective;

public:
	FORCEINLINE
		TArray<FResourceTransactionTicket> GetTransactionOrder() { return transactionOrder; };
	FORCEINLINE
		FTimerHandle GetTimerHandle() { return timerHandle; }
	FORCEINLINE
		AActor* GetGoalActor() { return goalActor; }
	FORCEINLINE
		ETransportOrderStatus GetTransportOrderStatus() { return transportOrderStatus; }
	FORCEINLINE
		AProductionsite* GetOwningProductionSite() { return owningProdSite; }
	FORCEINLINE
		ETransportatOrderDirecrtive GetOrderDirective() { return transportDirective; }
};

USTRUCT()
struct FTransportManagerSaveData
{
	GENERATED_BODY()

	FTransportManagerSaveData(){}

	FTransportManagerSaveData(TArray<FTransportOrder> _currentTransportOrders)
	{
		
	}

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
	UPROPERTY(VisibleAnywhere,Category=info,meta=(AllowPrivateAccess))
		TArray<FTransportOrder> allTransportOrders;

	UPROPERTY()
		class UProductionSiteManager* productionSiteManager;

	UPROPERTY()
		UWorld* world;

	UPROPERTY()
		AMarketManager* marketManager;
	UPROPERTY()
		class AAdvancedProjectCharacter* owningPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
		float transportSpeed;

public:
	UFUNCTION()
	void InitTransportManager(AMarketManager* _marketManager, UProductionSiteManager* _prodSiteManager, AAdvancedProjectCharacter* _owningPlayer);

	UFUNCTION(BlueprintCallable)
		void TestOrder();

private:
	UFUNCTION(BlueprintCallable)
		void CreateTransportOrder(TArray<FResourceTransactionTicket> _transaction, AActor* _goalActor, ETransportOrderStatus _orderStatus, AProductionsite* _owningSite, ETransportatOrderDirecrtive _transportDirective);

	UFUNCTION()
		void ManageTransaction(FTransportOrder _orderToHandle);
	UFUNCTION()
		void HandleMarketTransaction(FTransportOrder _orderToHandle);
	UFUNCTION()
		void HandleProdSiteTransaction(FTransportOrder _orderToHandle);

	// Ich will die menge an resourcen in meinem owner pool samplen wenn ich eine deduction order von ihr herraus sende und unter dem umstand die menge an resourcen <= 0 ist 0 geben u./o. < gewünschte menge den rest herrausgeben
	UFUNCTION()
		TArray<FResourceTransactionTicket> SampleSitePool(TArray<FResourceTransactionTicket> _ticketsToCheck, AProductionsite* _siteToSample);
};