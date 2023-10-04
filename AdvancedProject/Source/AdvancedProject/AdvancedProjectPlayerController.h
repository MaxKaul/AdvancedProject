// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "EnumLibrary.h"
#include "InputActionValue.h"
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
	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION()
		void RotateCamera();
	UFUNCTION()
		void ZoomCamera(const FInputActionValue& _value);
	UFUNCTION()
		void MoveRight(const FInputActionValue& _value);
	UFUNCTION()
		void MoveForward(const FInputActionValue& _value);

	UFUNCTION()
		bool NullcheckDependencies();

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
		UInputAction* zoomInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		UInputAction* zoomMoveRightInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		UInputAction* zoomMoveForwardInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		float rotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		float cameraMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		float zoomSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		FVector2D cameraZoomLenghtMinMax;
};