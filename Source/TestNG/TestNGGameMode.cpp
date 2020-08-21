// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TestNGGameMode.h"
#include "TestNGHUD.h"
#include "TestNGCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
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

void ATestNGGameMode::GivePointsToPlayer(const int32 MatchLength, APawn* Player)
{
    if ((MatchLength < 0) || (Player == NULL)) return;
	ATestNGCharacter* aux = Cast<ATestNGCharacter>(Player);
	if (aux) {
		int32 Points = CalculatePoints(MatchLength);
		aux->AddPoints(Points);
	}
	else UE_LOG(LogTemp, Error, TEXT("Is Not a ATestNGCharacter"));
}

void ATestNGGameMode::TestPoints()
{
	ACharacter* aux = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	GivePointsToPlayer(12, aux);
}