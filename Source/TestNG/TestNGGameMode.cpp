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

int32 ATestNGGameMode::CalculatePoints(int32 Cant)
{
	if (Cant <= 1)
		return Cant;
	return CalculatePoints(Cant - 1) + CalculatePoints(Cant - 2);
}

void ATestNGGameMode::GivePointsToPlayer(const int32 MatchLength, ACharacter* Player)
{
    if ((MatchLength <= 0) || (Player == NULL)) return;
	auto aux = Cast<ATestNGCharacter>(Player);
	if (aux) {
		int32 Points = CalculatePoints(MatchLength);
		aux->AddPoints(Points);
	}
}

