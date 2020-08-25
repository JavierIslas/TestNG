// Fill out your copyright notice in the Description page of Project Settings.


#include "ATNGPyramid.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "../Public/ATNGCube.h"
#include "TestNG/TestNGGameMode.h"

// Sets default values
AATNGPyramid::AATNGPyramid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CubeSize.Set(50.0f, 50.0f, 50.0f);

}


AATNGCube* AATNGPyramid::CreateCube(int32 Mat, FVector SpawnLocation, int32 SpawnTableAddress)
{
	//if (nullptr) { return nullptr; }
	//else
	//{
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
			NewCube->ChangeMaterial(MaterialsLib[Mat].Material, Mat);
			CubesInGame[SpawnTableAddress] = NewCube;
			return NewCube;
		}
	//}
}
//Testing probability method to balance the max punctuation of a game
int32 AATNGPyramid::SelectColor()
{
	return FMath::RandRange(0, 2);
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

			CreateCube(MatID, SpawnLocation, TableAddress);
		}

	}
}

FVector AATNGPyramid::GetLocationFromPyramidAddress(int32 Address) const
{
	FVector Center = GetActorLocation();
	FVector OutLocation = FVector(-(TableWidth * 0.5f) * CubeSize.X + (CubeSize.X * 0.5f), 0.0f, -(TableHeight * 0.5f) * CubeSize.Y + (CubeSize.Y * 0.5f));
	if (TableWidth > 0) {
		OutLocation.X += CubeSize.X * (float)(Address % TableWidth);
		OutLocation.Z += CubeSize.Y * (float)(Address / TableWidth);
		OutLocation += Center;

		return OutLocation;
	}

	return FVector::ZeroVector;
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
	if (TableWidth > 0) {
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
	}
	ReturnTableAddress = (InitialTableAddres + XOffset + (YOffset * TableWidth));
	if ((ReturnTableAddress >= 0) && (ReturnTableAddress < (TableWidth * TableHeight))) {
		return true;
	}

	return false;
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
		Cube->SetState(EState::S_Normal);
	}
}

void AATNGPyramid::FindNeighbors(AATNGCube* StartingCube)
{
	if (StartingCube->GetState() != EState::S_PendingDelete) {
		int32 NeighborAddress;
		AATNGCube* NeighborCube = nullptr;
		StartingCube->SetState(EState::S_PendingDelete);
		// Check verticals, then check horizontals.
		for (int32 Horizontal = 0; Horizontal < 2; ++Horizontal)
		{
			// Check negative direction, then check positive direction.
			for (int32 Direction = -1; Direction <= 1; Direction += 2)
			{
				int32 MaxGridOffset = Horizontal ? TableWidth : TableHeight;
				// Check that its still withing range on the matrix
				for (int32 GridOffset = 1; GridOffset < MaxGridOffset; ++GridOffset)
				{
					if (GetPyramidAddressWithOffset(StartingCube->GetPyramidPosition(), Direction * (Horizontal ? GridOffset : 0), Direction * (Horizontal ? 0 : GridOffset), NeighborAddress))
					{
						NeighborCube = GetCubeFromPyramidAddress(NeighborAddress);
						if (NeighborCube && (NeighborCube->GetCubeColor() == StartingCube->GetCubeColor()))
						{
							CubesBeingDestroyed.Add(NeighborCube);
							FindNeighbors(NeighborCube);
						}
						else  GridOffset = MaxGridOffset;
					}
				}
			}
		}
	}
	return;
}


void AATNGPyramid::UpdateCubesInGame(int32 Index)
{
	int32 auxIndex;
	if (GetPyramidAddressWithOffset(Index, 0, 1, auxIndex)) {
		if ((GetCubeFromPyramidAddress(auxIndex)) && (GetCubeFromPyramidAddress(auxIndex)->GetState() == EState::S_Normal)) {
			GetCubeFromPyramidAddress(auxIndex)->SetState(EState::S_Falling);
			UpdateCubesInGame(auxIndex);
		}
		else return;
	}

}

void AATNGPyramid::StartMachingCubes(AATNGCube* StartPoint, APawn* Player)
{
	CubesBeingDestroyed.Empty();
	ATestNGGameMode* GM = Cast<ATestNGGameMode>(GetWorld()->GetAuthGameMode());

	if (GM) {

		CubesBeingDestroyed.Add(StartPoint);
		FindNeighbors(StartPoint);
		
		GM->GivePointsToPlayer(CubesBeingDestroyed.Num(), Player);

		for (AATNGCube* Cube : CubesBeingDestroyed) {
			UpdateCubesInGame(Cube->GetPyramidPosition());
		}

		for (AATNGCube* Cube : CubesInGame) {
			if (Cube) {
				if (Cube->GetState() == EState::S_PendingDelete) Cube->Destroy();
				else if (Cube->GetState() == EState::S_Falling) {
					Cube->StartFalling();
				}
			}
		}
	}
}