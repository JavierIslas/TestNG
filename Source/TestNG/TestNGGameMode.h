// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TestNGGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimeActor, AActor*, KillerActor, AController*, KillerController);

UCLASS(minimalapi)
class ATestNGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATestNGGameMode();

	UPROPERTY(BlueprintAssignable, Category = "Game Mode")
	FOnActorKilled OnActorKilled;
};



