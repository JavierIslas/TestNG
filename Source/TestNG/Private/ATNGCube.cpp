// Fill out your copyright notice in the Description page of Project Settings.

#include "ATNGCube.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMesh.h"
#include "Engine/EngineTypes.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "../Public/ATNGPyramid.h"
#include "TestNG/TestNGGameMode.h"
#include "TestNG/TestNGProjectile.h"

// Sets default values
AATNGCube::AATNGCube()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));

	if (MeshComponent)
	{
		MeshComponent->SetNotifyRigidBodyCollision(true);

		MeshComponent->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
		MeshComponent->OnComponentHit.AddDynamic(this, &AATNGCube::OnCompHit);
		RootComponent = MeshComponent;
		RootComponent->SetMobility(EComponentMobility::Movable);
	}
	else RootComponent->SetMobility(EComponentMobility::Movable);

}

void AATNGCube::TickFalling()
{
	ATestNGGameMode* GM = Cast<ATestNGGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		check(PyramidOwner);
		check(TotalFallingTime > 0.0f);
		float FallCompleteFraction = (GetWorld()->GetTimeSeconds() - FallingStartTime) / TotalFallingTime;

		//Stop falling if we're at the final location. Otherwise, continue to move
		if (FallCompleteFraction >= 1.0f)
		{
			StopFalling();
		}
		else
		{
			FVector NewLocation = FMath::Lerp(FallingStartLocation, FallingEndLocation, FallCompleteFraction);
			SetActorLocation(NewLocation);
		}
	}
	else
	{
		//Error. Stop ticking this function. Move to the final location
		StopFalling();
	}
}

void AATNGCube::StartFalling(bool bUseCurrentWorldLocation)
{
	float FallDistance = 0;

	FallingStartTime = GetWorld()->GetTimeSeconds();
	FallingStartLocation = GetActorLocation();
	//Falls at a fixed rate of 60 FPS
	GetWorldTimerManager().SetTimer(TimerHandle_TickFalling, this, &AATNGCube::TickFalling, 0.01f, true);
	check(PyramidOwner);

	if (!bUseCurrentWorldLocation)
	{
		//Fall from where we are on the table to wher we are supposed to be on the table
		int32 YOffset = 0;
		int32 HeightAboveBottom = 1;
		while (true)
		{
			++YOffset;
			if (PyramidOwner->GetPyramidAddressWithOffset(GetPyramidPosition(), 0, -YOffset, LandingAddress))
			{
				if (AATNGCube* TileBelow = PyramidOwner->GetCubeFromPyramidAddress(LandingAddress))
				{
					//We're not off the table, so check to see what is in this space and reack to it
					if (TileBelow->CubeState == EState::S_Falling)
					{
						//This space contains a fallig thile, so contiue to fall through it, but note that the tile will land underneath us, so we need to leave a gap for it
						++HeightAboveBottom;
						continue;
					}
					else if (TileBelow->CubeState == EState::S_PendingDelete)
					{
						//This space contains a tile that is about to be deleted. WE can fall through this space freely
						continue;
					}
				}
				else
				{
					//The space below is empty, but is on the table. We can fall through this space freely
					continue;
				}
			}
			//This space is off the table or contains a tile that is staying. Go back one space and stop
			YOffset -= HeightAboveBottom;
			PyramidOwner->GetPyramidAddressWithOffset(GetPyramidPosition(), 0, -YOffset, LandingAddress);
			break;
		}
		FallDistance = PyramidOwner->CubeSize.Y * YOffset;
		FallingEndLocation = FallingStartLocation;
		FallingEndLocation.Z -= FallDistance;
	}
	else
	{
		//Fall from where we are physically to where we are supposed to be on the table
		LandingAddress = GetPyramidPosition();
		FallingEndLocation = PyramidOwner->GetLocationFromPyramidAddress(LandingAddress);
		FallDistance = FallingStartLocation.Z - FallingEndLocation.Z;
	}
	/**
	ATestNGGameMode* GM = Cast<ATestNGGameMode>(UGameplayStatics::GetGameMode(this));
	TotalFallingTime = 0.0f;
	if (GM && (GM->TileMoveSpeed > 0.0f))
	{
		TotalFallingTime = FallDistance / 60;
	}
	if (TotalFallingTime <= 0.0f)
	{
		TotalFallingTime = 0.75f;
	}
	**/
}

void AATNGCube::StopFalling()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TickFalling);
	SetActorLocation(FallingEndLocation);
	PyramidOwner->OnFinishedFalling(this, LandingAddress);
}

void AATNGCube::SetPyramidAddress(int32 NewLocation)
{
	PyramidAddress = NewLocation;
}

int32 AATNGCube::GetPyramidPosition() const
{
	return PyramidAddress;
}

void AATNGCube::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ATestNGProjectile* aux = Cast<ATestNGProjectile>(OtherActor);
	if (aux)
	{
		OtherActor->Destroy();
		Destroy();
	}
}

void AATNGCube::ChangeMaterial(UMaterialInterface* Mat, int32 ColorID)
{
	MeshComponent->SetMaterial(0, Mat);
	CubeColor = ColorID;
}

// Called when the game starts or when spawned
void AATNGCube::BeginPlay()
{
	Super::BeginPlay();

	PyramidOwner = Cast<AATNGPyramid>(GetOwner());
	
}