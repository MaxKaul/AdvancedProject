// Copyright Epic Games, Inc. All Rights Reserved.

#include "AdvancedProjectCharacter.h"

#include "AdvancedProjectPlayerController.h"
#include "Builder.h"
#include "BuildingSite.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProductionSiteManager.h"
#include "MarketManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

AAdvancedProjectCharacter::AAdvancedProjectCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; 

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AAdvancedProjectCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AAdvancedProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	aspController = Cast<AAdvancedProjectPlayerController>(GetController());
	aspController->InitPlayerController(this);
}

void AAdvancedProjectCharacter::BuildTestProductionSite(ABuildingSite* _chosenSite)
{
	builder->BuildProductionSite(testMesh,EProductionSiteType::PST_Hardwood_Resin, _chosenSite, marketManager, productionSiteManager->GetAllProductionSites().Num() + 1);
}

void AAdvancedProjectCharacter::SetDisplayProductionSiteInfo(bool _status, AProductionsite* _hoverProdSite)
{
	viewProductionSite = _hoverProdSite;
	ToggleProdSiteInfoStatus();
}

AProductionsite* AAdvancedProjectCharacter::GetViewProductionSiteInfo()
{
	if (!viewProductionSite)
	{
		UE_LOG(LogTemp, Warning, TEXT("FProductionSiteDisplayInfos, !viewProductionSite"));
		return nullptr;
	}

	return viewProductionSite;
}

void AAdvancedProjectCharacter::ToggleProdSiteInfoStatus()
{
	bCanOpenProdSite = !bCanOpenProdSite;
}