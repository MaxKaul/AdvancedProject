// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AdvancedProjectCharacter.generated.h"

UCLASS(Blueprintable)
class AAdvancedProjectCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAdvancedProjectCharacter();

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION()
		void InitPlayer(AMarketManager* _marketManager);

	UFUNCTION(BlueprintCallable)
		void BuildTestProductionSite(class ABuildingSite* _chosenSite);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
	class UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
		class AMarketManager* marketManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
		class AAdvancedProjectPlayerController* aspController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
		class UProductionSiteManager* ProductionSiteManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
		class UBuilder* builder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess ))
		UStaticMesh* testMesh;
};

