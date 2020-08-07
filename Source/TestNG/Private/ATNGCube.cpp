// Fill out your copyright notice in the Description page of Project Settings.


#include "ATNGCube.h"

// Sets default values
AATNGCube::AATNGCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AATNGCube::TickFalling()
{
}

void AATNGCube::StopFalling()
{
}

void AATNGCube::SetTableAddress(int32 NewLocation)
{
}

int32 AATNGCube::GetPyramidPosition() const
{
	return int32();
}

// Called when the game starts or when spawned
void AATNGCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AATNGCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

