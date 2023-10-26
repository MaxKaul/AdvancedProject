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
#include "Engine/World.h"
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

	builder = CreateDefaultSubobject<UBuilder>("Builder");
	ProductionSiteManager = CreateDefaultSubobject<UProductionSiteManager>("ProductionSiteManager");
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

void AAdvancedProjectCharacter::InitPlayer(AMarketManager* _marketManager)
{
	marketManager = _marketManager;
	
	builder->InitBuilder(ProductionSiteManager, marketManager);
	ProductionSiteManager->InitProductionSiteManager(this, marketManager);
}

void AAdvancedProjectCharacter::BuildTestProductionSite(ABuildingSite* _chosenSite)
{
	builder->BuildProductionSite(testMesh,EProductionSiteType::PST_Hardwood_Resin, _chosenSite, marketManager, 0);
}