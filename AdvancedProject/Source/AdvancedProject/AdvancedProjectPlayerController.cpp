// Copyright Epic Games, Inc. All Rights Reserved.

#include "AdvancedProjectPlayerController.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AdvancedProjectCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

AAdvancedProjectPlayerController::AAdvancedProjectPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	rotationSpeed = 100.f;
	zoomSpeed = 100.f;

	cameraZoomLenghtMinMax = FVector2D{ -2000,1000 };
}

void AAdvancedProjectPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!NullcheckDependencies())
	{
		UE_LOG(LogTemp, Warning, TEXT("AAdvancedProjectPlayerController, !NullcheckDependencies"));
		return;
	}
}

void AAdvancedProjectPlayerController::InitPlayerController(AAdvancedProjectCharacter* _controllerOwner)
{
	controllerOwner = _controllerOwner;
	CameraBoom = controllerOwner->GetCameraBoom();

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(this, false);


	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void AAdvancedProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(zoomMoveRightInputAction, ETriggerEvent::Triggered, this, &AAdvancedProjectPlayerController::MoveRight);
		EnhancedInputComponent->BindAction(zoomMoveForwardInputAction, ETriggerEvent::Triggered, this, &AAdvancedProjectPlayerController::MoveForward);
		EnhancedInputComponent->BindAction(zoomInputAction, ETriggerEvent::Triggered, this, &AAdvancedProjectPlayerController::ZoomCamera);
		EnhancedInputComponent->BindAction(rotateInputAction, ETriggerEvent::Ongoing, this, &AAdvancedProjectPlayerController::RotateCamera);


		EnhancedInputComponent->BindAction(zoomInputAction, ETriggerEvent::Completed , this, &AAdvancedProjectPlayerController::ZoomCamera);
		EnhancedInputComponent->BindAction(rotateInputAction, ETriggerEvent::Completed, this, &AAdvancedProjectPlayerController::RotateCamera);
	}
}

void AAdvancedProjectPlayerController::RotateCamera()
{
	float deltaX;
	float deltaY;

	GetInputMouseDelta(deltaX, deltaY);

	if (deltaX == 0 && deltaY == 0)
		return;

	float  newyaw = CameraBoom->GetComponentRotation().Yaw;

	if(deltaX > 0)
		newyaw += rotationSpeed * GetWorld()->GetDeltaSeconds();
	else
		newyaw -= rotationSpeed * GetWorld()->GetDeltaSeconds();

	controllerOwner->SetActorRotation(FRotator(controllerOwner->GetActorRotation().Pitch, newyaw, controllerOwner->GetActorRotation().Roll));
	CameraBoom->SetRelativeRotation(FRotator(CameraBoom->GetComponentRotation().Pitch, newyaw, CameraBoom->GetComponentRotation().Roll), false);

	UE_LOG(LogTemp,Warning,TEXT("FDDD"))
}

void AAdvancedProjectPlayerController::MoveRight(const FInputActionValue& _value)
{
	float movemefloatright = _value.Get<float>();

	if (controllerOwner != nullptr)
	{
		const FRotator rotation = controllerOwner->GetActorRotation();
		const FRotator yawrotation(0, rotation.Yaw, 0);

		const FVector rightdirection = FRotationMatrix(yawrotation).GetUnitAxis(EAxis::Y);

		controllerOwner->AddMovementInput(rightdirection, movemefloatright);
	}
}

void AAdvancedProjectPlayerController::MoveForward(const FInputActionValue& _value)
{
	float movemefloatforward = _value.Get<float>();

	if (controllerOwner != nullptr)
	{
		const FRotator rotation = controllerOwner->GetActorRotation();
		const FRotator yawrotation(0, rotation.Yaw, 0);

		const FVector forwarddirection = FRotationMatrix(yawrotation).GetUnitAxis(EAxis::X);

		controllerOwner->AddMovementInput(forwarddirection, movemefloatforward);
	}
}

void AAdvancedProjectPlayerController::ZoomCamera(const FInputActionValue& _value)
{
	if (_value.Get<float>() == 0)
		return;

	FVector offset = CameraBoom->SocketOffset;

	if(_value.Get<float>() == 1 && offset.X > cameraZoomLenghtMinMax.X)
		offset.X -= zoomSpeed;
	else if(_value.Get<float>() == -1 && offset.X < cameraZoomLenghtMinMax.Y)
		offset.X += zoomSpeed;

	CameraBoom->SocketOffset = offset;
}


bool AAdvancedProjectPlayerController::NullcheckDependencies()
{
	bool status = true;

	if(!controllerOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("AAdvancedProjectPlayerController, !ControllerOwner"));
		status = false;
	}

	if (!CameraBoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("AAdvancedProjectPlayerController, !ControllerOwner"));
		status = false;
	}

	return status;
}