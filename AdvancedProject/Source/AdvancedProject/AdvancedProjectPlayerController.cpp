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

	cameraZoomLenghtMinMax = FVector2D{ 500,10000};
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

	if (zoomCurveTimeline.IsPlaying())
		zoomCurveTimeline.TickTimeline(DeltaSeconds);

	UE_LOG(LogTemp,Warning,TEXT("%f"), zoomSpeed)
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

void AAdvancedProjectPlayerController::MoveCamera()
{
}

void AAdvancedProjectPlayerController::ZoomCamera(const FInputActionValue& _value)
{
	if (zoomCurveTimeline.IsPlaying())
		return;


	if(_value.Get<float>() == 1)
		zoomStatus = ECameraZoomStatus::ETS_ZoomOut;
	else if(_value.Get<float>() == -1)
		zoomStatus = ECameraZoomStatus::ETS_ZoomIn;
	else 
		zoomStatus = ECameraZoomStatus::ETS_NoZoom;

	BeginZoomTimeline();

}

void AAdvancedProjectPlayerController::BeginZoomTimeline()
{
	FOnTimelineFloat timelineprogress;

	timelineprogress.BindUFunction(this, FName("ZoomTimelineProgress"));

	zoomCurveTimeline.SetTimelineLengthMode(TL_LastKeyFrame);
	zoomCurveTimeline.SetPlayRate(zoomSpeed);
	zoomCurveTimeline.SetLooping(false);
	zoomCurveTimeline.AddInterpFloat(zoomCurveFloat, timelineprogress);

	zoomCurveTimeline.PlayFromStart();
}

void AAdvancedProjectPlayerController::ZoomTimelineProgress(float _timelineAlpha)
{
	if (zoomStatus == ECameraZoomStatus::ETS_ZoomIn && CameraBoom->TargetArmLength >= cameraZoomLenghtMinMax.X)
		CameraBoom->TargetArmLength -= _timelineAlpha * GetWorld()->GetDeltaSeconds();
	else if (zoomStatus == ECameraZoomStatus::ETS_ZoomOut && CameraBoom->TargetArmLength <= cameraZoomLenghtMinMax.Y)
		CameraBoom->TargetArmLength += _timelineAlpha * GetWorld()->GetDeltaSeconds();
	else
		zoomCurveTimeline.Stop();
}