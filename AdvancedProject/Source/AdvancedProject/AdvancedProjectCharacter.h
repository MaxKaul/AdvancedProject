// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "AdvancedProjectCharacter.generated.h"

UCLASS(Blueprintable)
class AAdvancedProjectCharacter : public APlayerBase
{
	GENERATED_BODY()

public:
	AAdvancedProjectCharacter();

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	FORCEINLINE
	class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE
	class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintCallable)FORCEINLINE
		bool GetOpenProdSitePanel() {return bCanOpenProdSite ;}

	UFUNCTION(BlueprintCallable)
		void BuildTestProductionSite(class ABuildingSite* _chosenSite);
	UFUNCTION(BlueprintCallable)
		void SetDisplayProductionSiteInfo(bool _status, AProductionsite* _hoverProdSite);

	UFUNCTION(BlueprintCallable)
		AProductionsite* GetViewProductionSiteInfo();

	UFUNCTION(BlueprintCallable)
		void ToggleProdSiteInfoStatus();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
	class UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
	class USpringArmComponent* CameraBoom;

	UPROPERTY()
		bool bCanOpenProdSite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
		class AAdvancedProjectPlayerController* aspController;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Info,meta=(AllowPrivateAccess))
		class AProductionSiteWorldManager* prodSiteWorldManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
		UStaticMesh* testMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
		UDataTable* productionSideDisplayInfo;

	// Muss noch auf hover umgestellt werden
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Info, meta = (AllowPrivateAccess))
	AProductionsite* viewProductionSite;
};

