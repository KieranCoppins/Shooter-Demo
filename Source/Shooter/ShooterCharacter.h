// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GenericTeamAgentInterface.h"
#include "Perception/AISightTargetInterface.h"

#include "Kismet/GameplayStatics.h"
#include "EnemyCharacter.h"
#include "EnemyController.h"

#include "ShooterCharacter.generated.h"

UCLASS(config=Game)
class AShooterCharacter : public ACharacter, public IGenericTeamAgentInterface, public IAISightTargetInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

private:
	FGenericTeamId teamID;

public:
	AShooterCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseLookUpRate;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = PublicVars)
		bool bAiming;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Stats)
		float health;

	UPROPERTY(EditAnywhere, Category = PublicVars)
		TSubclassOf<class ABullet> BulletBP;

	UPROPERTY (EditAnywhere, Category = PublicVars)
		FVector MuzzleOffset;


	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom () const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera () const { return FollowCamera; }

	//Called every frame
	virtual void Tick (float DeltaTime) override;

	virtual FGenericTeamId GetGenericTeamId () const override;

	//Override from IAISightTargetInterface to adjust hit target
	virtual bool CanBeSeenFrom (const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = nullptr, const bool* bWasVisible = nullptr, int32* UserData = nullptr) const override;


protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	//Rotates the character smoothly to the camera's forward vector
	void SmoothRotate (float DeltaTime, float interpSpeed);

	//Follows the player smoothly by interpolating the boom arm distance
	void SmoothFollow (float DeltaTime, float interpSpeed);

	//Crouch the player
	void DoCrouch ();

	//Aim
	void DoAim ();

	//Aim in
	void AimIn ();

	//Aim Out
	void AimOut ();

	//Handles firing the weapon
	void Shoot ();

	void UpdateHUDVars ();

	void DEBUG_ShootAtAllEnemies ();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
};

