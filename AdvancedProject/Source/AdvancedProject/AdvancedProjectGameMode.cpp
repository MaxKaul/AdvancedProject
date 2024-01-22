// Copyright Epic Games, Inc. All Rights Reserved.

#include "AdvancedProjectGameMode.h"
#include "AdvancedProjectPlayerController.h"
#include "AdvancedProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAdvancedProjectGameMode::AAdvancedProjectGameMode()
{
	PlayerControllerClass = AAdvancedProjectPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/GameContent/Blueprints/Player/BP_ASPPlayer"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/GameContent/Blueprints/Player/BP_ASPPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}