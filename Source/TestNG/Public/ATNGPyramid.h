// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATNGPyramid.generated.h"

/*
Crear una estructura que represente el cubo y replicarla
*/

class AATNGCube; 

USTRUCT(BlueprintType)
struct FTileType
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Probability = 0.33;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterialInterface* Material = nullptr;

};

UCLASS()
class TESTNG_API AATNGPyramid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AATNGPyramid();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AATNGCube*> CubesInGame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConfigCube")
	TSubclassOf<AATNGCube> CubeToSpawn;

	/** Size of a position on the Table. Doesn't have borders or spacing between tiles */
	UPROPERTY(EditDefaultsOnly, Category = "ConfigCubes")
	FVector CubeSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConfigTable")
	TArray<FTileType> MaterialsLib;

	/** The width of the Table. Needed to Calculate cube position and neighbors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConfigTable")
	int32 TableWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConfigTable")
	int32 TableHeight;

	/** Spawn a tile and associate it with a specific Table address */
	AATNGCube* CreateCube(int32 Mat, FVector SpawnLocation, int32 SpawnTableAddress);

	/** Randomly select a color for the cube, using the probability values on the tile. */
	int32 SelectColor();

	/** Get the pointer to the tile ar the specofoed grod address. */
	AATNGCube* GetCubeFromPyramidAddress(int32 TableAddress) const;

	UFUNCTION(BlueprintCallable, Category = "Initialization")
	void InitializePyramid();

	/** Get world location from a Table address */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	FVector GetLocationFromPyramidAddress(int32 Address) const;

	/** Get world location from a Table address relative to another address. Offset between both is measured in tiles */
	FVector GetLocationFromPyramidAddressWithOffset(int32 TableAddress, int32 XOffserInTiles, int32 YOffsetInTiles) const;

	/** Get table address relative to another table address. Offset between both is measured in tiles */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	bool GetPyramidAddressWithOffset(int32 InitialTableAddres, int32 XOffset, int32 YOffset, int32& ReturnTableAddress) const;

	/** Determine if 2 table addresses are valid and adjacent */
	bool AreAddressesNeighbors(int32 TableAddressA, int32 TableAddressB) const;

	void OnFinishedFalling(AATNGCube* Tile, int32 LandingAddress);

	TArray<AATNGCube*> FindNeighbors(AATNGCube* StartingTile, bool bMustMatchID, int32 RunLength) const;

private:
	/** Tiles that are currently falling */
	TArray<AATNGCube*> FallingTiles;

	/** Tiles that are currentle swapping position with each other.Sholud be exactly two of them, or zero */
	TArray<AATNGCube*> SwappingTiles;

	/** After spawning new tiles, wich tiles to check for automatic matches */
	TArray<AATNGCube*> TilesToCheck;

	/** Tiles that are currently reacting to being matches */
	TArray<AATNGCube*> TilesBeingDestroyed;

};
