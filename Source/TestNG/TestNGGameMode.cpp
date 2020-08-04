// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TestNGGameMode.h"
#include "TestNGHUD.h"
#include "TestNGCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATestNGGameMode::ATestNGGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ATestNGHUD::StaticClass();
}
