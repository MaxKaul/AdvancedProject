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

	virtual void InitPlayer(FPlayerSaveData _saveData, AMarketManager* _marketManager) override;

	UFUNCTION(BlueprintCallable)
		void BuildTestProductionSite(class ABuildingSite* _chosenSite);
	UFUNCTION(BlueprintCallable)
		void SetDisplayProductionSiteInfo(bool _status);
	UFUNCTION(BlueprintCallable)
		bool GetDisplayProductionSiteInfo();

protected:
		virtual FPlayerSaveData GetPlayerSaveData() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
	class UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
		class AMarketManager* marketManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
		class AAdvancedProjectPlayerController* aspController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
		class UBuilder* builder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
		UStaticMesh* testMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info, meta = (AllowPrivateAccess))
		bool bCanOpenProductionSiteInfo;
};

