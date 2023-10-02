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
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION()
		void RotateCamera();
	UFUNCTION()
		void ZoomCamera(const FInputActionValue& _value);
	UFUNCTION()
		void MoveCamera();

	UFUNCTION()
		void BeginZoomTimeline();

	UFUNCTION()
		void ZoomTimelineProgress(float _timelineAlpha);

private:
	UPROPERTY()
		class AAdvancedProjectCharacter* controllerOwner;

	UPROPERTY()
		class USpringArmComponent* CameraBoom;

	FTimeline zoomCurveTimeline;

	UPROPERTY(EditAnywhere, Category = Timeline, meta = (AllowPrivateAccess))
		UCurveFloat* zoomCurveFloat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		class UInputAction* rotateInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		UInputAction* zoomInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		UInputAction* cameraMoveInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		float rotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		float cameraMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		float zoomSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
		FVector2D cameraZoomLenghtMinMax;

	UPROPERTY()
		ECameraZoomStatus zoomStatus;

};


