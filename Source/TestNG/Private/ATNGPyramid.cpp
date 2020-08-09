// Fill out your copyright notice in the Description page of Project Settings.


#include "ATNGPyramid.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "../Public/ATNGCube.h"

// Sets default values
AATNGPyramid::AATNGPyramid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CubeSize.Set(50.0f, 50.0f, 50.0f);

}


AATNGCube* AATNGPyramid::CreateCube(UMaterialInterface* TileMaterial, FVector SpawnLocation, int32 SpawnTableAddress, int32 TileTypeID)
{
	if (nullptr) { return nullptr; }
	else
	{
		checkSlow(MaterialsLib.IsValidIndex(TileTypeID));

		UWorld* const World = GetWorld();
		if (!World) { return nullptr; }

		else
		{
			//Set Spawn parameters
			FActorSpawnParameters SpawnParam;
			SpawnParam.Owner = this;
			SpawnParam.Instigator = Instigator;
			SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			//Cube never rotate
			FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

			//Spawn the cube
			AATNGCube* const NewCube = World->SpawnActor<AATNGCube>(CubeToSpawn, SpawnLocation, SpawnRotation, SpawnParam);
			NewCube->SetPyramidAddress(SpawnTableAddress);
			CubesInGame[SpawnTableAddress] = NewCube;
			return NewCube;
		}
	}
}

int32 AATNGPyramid::SelectColor()
{
	return int32();
}

AATNGCube* AATNGPyramid::GetCubeFromPyramidAddress(int32 TableAddress) const
{
	if (CubesInGame.IsValidIndex(TableAddress))
	{
		return CubesInGame[TableAddress];
	}
	return nullptr;
}

void AATNGPyramid::InitializePyramid()
{
	CubesInGame.Empty(TableWidth * TableHeight);
	CubesInGame.AddZeroed(CubesInGame.Max());
	FVector SpawnLocation;

	for (int32 Column = 0; Column < TableWidth; ++Column)
	{
		for (int32 Row = 0; Row < TableHeight; ++Row)
		{
			int32 MatID = SelectColor();
			int32 TableAddress;
			GetPyramidAddressWithOffset(0, Column, Row, TableAddress);
			SpawnLocation = GetLocationFromPyramidAddress(TableAddress);

			CreateCube(MaterialsLib[MatID], SpawnLocation, TableAddress, MatID);
		}

	}
}

FVector AATNGPyramid::GetLocationFromPyramidAddress(int32 Address) const
{
	FVector Center = GetActorLocation();
	FVector OutLocation = FVector(-(TableWidth * 0.5f) * CubeSize.X + (CubeSize.X * 0.5f), 0.0f, -(TableHeight * 0.5f) * CubeSize.Y + (CubeSize.Y * 0.5f));
	check(TableWidth > 0);
	OutLocation.X += CubeSize.X * (float)(Address % TableWidth);
	OutLocation.Z += CubeSize.Y * (float)(Address / TableWidth);
	OutLocation += Center;

	return OutLocation;
}

FVector AATNGPyramid::GetLocationFromPyramidAddressWithOffset(int32 TableAddress, int32 XOffserInTiles, int32 YOffsetInTiles) const
{
	FVector OutLocation = GetLocationFromPyramidAddress(TableAddress);
	OutLocation.X += CubeSize.X * (float)(XOffserInTiles);
	OutLocation.Y += CubeSize.Y * (float)(YOffsetInTiles);
	return OutLocation;
}

bool AATNGPyramid::GetPyramidAddressWithOffset(int32 InitialTableAddres, int32 XOffset, int32 YOffset, int32& ReturnTableAddress) const
{
	int32 NewAxisValue;

	ReturnTableAddress = -1; //Invalid value

	//check for within X range
	check(TableWidth > 0);
	NewAxisValue = (InitialTableAddres % TableWidth) + XOffset;
	if (NewAxisValue != FMath::Clamp(NewAxisValue, 0, (TableWidth - 1)))
	{
		return false;
	}

	//check for within Y range
	NewAxisValue = (InitialTableAddres % TableWidth) + YOffset;
	if (NewAxisValue != FMath::Clamp(NewAxisValue, 0, (TableHeight - 1)))
	{
		return false;
	}

	ReturnTableAddress = (InitialTableAddres + XOffset + (YOffset * TableWidth));
	check(ReturnTableAddress >= 0);
	check(ReturnTableAddress < (TableWidth* TableHeight));

	return true;
}

bool AATNGPyramid::AreAddressesNeighbors(int32 TableAddressA, int32 TableAddressB) const
{
	if ((FMath::Min(TableAddressA, TableAddressB) >= 0) && (FMath::Max(TableAddressA, TableAddressB) < (TableWidth * TableHeight)))
	{
		int32 TableAddressOffset = FMath::Abs(TableAddressA - TableAddressB);
		return ((TableAddressOffset == 1) || (TableAddressOffset == TableWidth));
	}
	return false;
}

void AATNGPyramid::OnFinishedFalling(AATNGCube* Cube, int32 LandingAddress)
{
	int32 ReturnTableAddress;

	//Remove Tile from its original position if it's still there (hasn't been replaced by another falling tile)
	if (GetPyramidAddressWithOffset(Cube->GetPyramidPosition(), 0, 0, ReturnTableAddress))
	{
		if (CubesInGame[ReturnTableAddress] == Cube)
		{
			CubesInGame[ReturnTableAddress] = nullptr;
		}
	}

	//Validate new table address and replace whatever is there
	if (GetPyramidAddressWithOffset(LandingAddress, 0, 0, ReturnTableAddress))
	{
		CubesInGame[ReturnTableAddress] = Cube;
		Cube->SetPyramidAddress(ReturnTableAddress);
		Cube->CubeState = EState::S_Normal;
	}
}
