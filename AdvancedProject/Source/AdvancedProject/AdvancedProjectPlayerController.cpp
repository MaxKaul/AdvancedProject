// Copyright Epic Games, Inc. All Rights Reserved.

#include "AdvancedProjectPlayerController.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AdvancedProjectCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
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

void AAdvancedProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void AAdvancedProjectPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AAdvancedProjectPlayerController::InitPlayerController(AAdvancedProjectCharacter* _controllerOwner)
{
	controllerOwner = _controllerOwner;
	CameraBoom = controllerOwner->GetCameraBoom();
}

void AAdvancedProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(rotateInputAction, ETriggerEvent::Ongoing, this, &AAdvancedProjectPlayerController::RotateCamera);

		EnhancedInputComponent->BindAction(zoomInputAction, ETriggerEvent::Triggered, this, &AAdvancedProjectPlayerController::ZoomCamera);
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

	FHitResult hit;

	CameraBoom->K2_SetRelativeRotation(FRotator(CameraBoom->GetComponentRotation().Pitch, newyaw, CameraBoom->GetComponentRotation().Roll), false, hit, false);
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