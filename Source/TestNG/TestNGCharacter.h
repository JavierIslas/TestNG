// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TestNGCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class ATestNGProjectile;
class USoundBase;
class UAnimSequence;

UCLASS(config = Game)
class ATestNGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATestNGCharacter();

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ATestNGProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	UAnimSequence* FireAnimation;

	UPROPERTY(Replicated)
	bool bIsAlive;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentPoints();

	UFUNCTION(BlueprintCallable)
	void AddPoints(int32 NewPoints);

protected:

	/** Fires a projectile. */
	void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/** Pawn mesh: 1st person view  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* Mesh1PComponent;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* GunMeshComponent;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Points")
	int32 CurrentPoints;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1PComponent; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return CameraComponent; }

	virtual void Tick(float DeltaTime) override;
};
