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

	builder = CreateDefaultSubobject<UBuilder>("Builder");
	productionSiteManager = CreateDefaultSubobject<UProductionSiteManager>("ProductionSiteManager");
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

// Das meiste an Implementation könnte in die base rein
void AAdvancedProjectCharacter::InitPlayer(FPlayerSaveData _saveData, AMarketManager* _marketManager, AWorkerWorldManager* _workerWorldManager)
{
	marketManager = _marketManager;
	workerWorldManager = _workerWorldManager;

	SetActorLocation(_saveData.GetLoaction());
	SetActorRotation(_saveData.GetRotation());

	builder->InitBuilder(productionSiteManager, marketManager, this);
	productionSiteManager->InitProductionSiteManager(this, _saveData.GetProductionSiteManagerSaveData() ,marketManager, workerWorldManager);
}

void AAdvancedProjectCharacter::BuildTestProductionSite(ABuildingSite* _chosenSite)
{
	builder->BuildProductionSite(testMesh,EProductionSiteType::PST_Hardwood_Resin, _chosenSite, marketManager, productionSiteManager->GetAllProductionSites().Num() + 1);
}

void AAdvancedProjectCharacter::SetDisplayProductionSiteInfo(bool _status, AProductionsite* _hoverProdSite)
{
	viewProductionSite = _hoverProdSite;
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

FPlayerSaveData AAdvancedProjectCharacter::GetPlayerSaveData()
{
	FPlayerSaveData savedata;

	savedata.InitSaveData(GetActorLocation(), GetActorRotation(), productionSiteManager->GetProductionSiteManagerSaveData());

	return savedata;
}

