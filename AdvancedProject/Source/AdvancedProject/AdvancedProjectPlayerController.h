// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AdvancedProjectPlayerController.generated.h"

UCLASS()
class AAdvancedProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAdvancedProjectPlayerController();

	void InitPlayerController(class AAdvancedProjectCharacter* _controllerOwner);

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay();

private:
	UFUNCTION()
		void RotateCamera();
	UFUNCTION()
		void ZoomCameraIn(const FInputActionValue& _value);
	UFUNCTION()
		void ZoomCameraOut(const FInputActionValue& _value);
	UFUNCTION()
		void MoveCamera();

private:
	UPROPERTY()
		class AAdvancedProjectCharacter* controllerOwner;

	UPROPERTY()
		class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		class UInputAction* rotateInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		UInputAction* zoomInInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		UInputAction* zoomOutInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		UInputAction* cameraMoveInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		float rotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		float cameraMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		float zoomSpeed;
};


