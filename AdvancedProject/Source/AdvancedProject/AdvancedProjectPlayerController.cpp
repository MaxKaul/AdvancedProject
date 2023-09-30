// Copyright Epic Games, Inc. All Rights Reserved.

#include "AdvancedProjectPlayerController.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AdvancedProjectCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

AAdvancedProjectPlayerController::AAdvancedProjectPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	rotationSpeed = 1.f;
	zoomSpeed = 1.f;
}

void AAdvancedProjectPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
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
		EnhancedInputComponent->BindAction(cameraMoveInputAction, ETriggerEvent::Ongoing, this, &AAdvancedProjectPlayerController::MoveCamera);
		EnhancedInputComponent->BindAction(zoomInInputAction, ETriggerEvent::Triggered, this, &AAdvancedProjectPlayerController::ZoomCameraIn);
		EnhancedInputComponent->BindAction(zoomOutInputAction, ETriggerEvent::Triggered, this, &AAdvancedProjectPlayerController::ZoomCameraOut);
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

void AAdvancedProjectPlayerController::MoveCamera()
{
}

void AAdvancedProjectPlayerController::ZoomCameraOut(const FInputActionValue& _value)
{
	FVector2D ttt = _value.Get<FVector2D>();

	UE_LOG(LogTemp, Warning, TEXT("%f"), ttt.X)

	CameraBoom->TargetArmLength += zoomSpeed * GetWorld()->GetDeltaSeconds();
}

void AAdvancedProjectPlayerController::ZoomCameraIn(const FInputActionValue& _value)
{
	FVector2D ttt = _value.Get<FVector2D>();

	UE_LOG(LogTemp,Warning,TEXT("%f"), ttt.X)

	CameraBoom->TargetArmLength -= zoomSpeed * GetWorld()->GetDeltaSeconds();
}
