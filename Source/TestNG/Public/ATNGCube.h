// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATNGCube.generated.h"

UENUM()
namespace EState
{
	enum Type
	{
		S_Normal,
		S_Falling,
		S_PendingDelete
	};
}

UCLASS()
class TESTNG_API AATNGCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AATNGCube();

	void TickFalling();

	void StartFalling(bool bUseCurrentWorldLocation = false);

	void StopFalling();

	void SetPyramidAddress(int32 NewLocation);

	int32 GetPyramidPosition() const;

	//class UStaticMesh* GetMeshComponent() { return MeshComponent; };

	UFUNCTION(BlueprintImplementableEvent)
	void ChangeMaterial(class UMaterialInterface* Mat);

	UPROPERTY(BlueprintReadOnly)
	int32 CubeColor;

	TEnumAsByte<EState::Type> CubeState;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector FallingStartLocation;

	FVector FallingEndLocation;

	FTimerHandle TimerHandle_TickFalling;

	class UStaticMesh* MeshComponent;

	float TotalFallingTime;

	float FallingStartTime;

	/** Location in the table as a 1D value, the table can translate it to a position*/
	UPROPERTY(BlueprintReadOnly, Category = "Tile")
	int32 PyramidAddress;

	/** Location in the table where will land as a 1D value, the table can translate it to a position. Used while falling.*/
	int32 LandingAddress;

	/**Table where the Tile is */
	UPROPERTY(BlueprintReadOnly, Category = "Tile")
	class AATNGPyramid* PyramidOwner;

};
