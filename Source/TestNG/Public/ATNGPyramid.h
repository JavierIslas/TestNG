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
struct FCubeType
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
	TArray<FCubeType> MaterialsLib;

	/** The width of the Table. Needed to Calculate cube position and neighbors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConfigTable")
	int32 TableWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConfigTable")
	int32 TableHeight;

	/** Spawn a tile and associate it with a specific Table address */
	AATNGCube* CreateCube(int32 Mat, FVector SpawnLocation, int32 SpawnTableAddress);

	/** Randomly select a color for the cube, using the probability values on the FCubeType struct. */
	int32 SelectColor();

	AATNGCube* GetCubeFromPyramidAddress(int32 TableAddress) const;

	UFUNCTION(BlueprintCallable, Category = "Initialization")
	void InitializePyramid();

	/** Get world location from a Table address */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	FVector GetLocationFromPyramidAddress(int32 Address) const;

	/** Get world location from a Table address relative to another address. Offset between both is measured in cubes */
	FVector GetLocationFromPyramidAddressWithOffset(int32 TableAddress, int32 XOffserInTiles, int32 YOffsetInTiles) const;

	/** Get table address relative to another table address. Offset between both is measured in cubes */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	bool GetPyramidAddressWithOffset(int32 InitialTableAddres, int32 XOffset, int32 YOffset, int32& ReturnTableAddress) const;

	/** Determine if 2 table addresses are valid and adjacent */
	bool AreAddressesNeighbors(int32 TableAddressA, int32 TableAddressB) const;

	void OnFinishedFalling(AATNGCube* Tile, int32 LandingAddress);


	void StartMachingCubes(AATNGCube* StartPoint, class APawn* Player);

private:
	/** Array with cubes that need to change position */
	TArray<AATNGCube*> FallingCubes;

	/** Cubes that need to be destroided */
	TArray<AATNGCube*> CubesBeingDestroyed;

	/** Change the state or destruct the cubes that are in the game*/
	void UpdateCubesInGame(int32 Index);

	/** Look for all the adyacent cubes with the same color to match*/
	void FindNeighbors(AATNGCube* StartingTile);

	
};
