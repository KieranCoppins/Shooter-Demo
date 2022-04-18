// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Bullet.h"
#include "CharacterHUD.h"

#include "Kismet/KismetMathLibrary.h"

//////////////////////////////////////////////////////////////////////////
// AShooterCharacter

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//Dont rotate our character with our controller - Smooth Rotate will handle that
	bUseControllerRotationYaw = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	bAiming = false;

	teamID = FGenericTeamId (0);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

	PlayerInputComponent->BindAction ("Crouch", IE_Pressed, this, &AShooterCharacter::DoCrouch);

	PlayerInputComponent->BindAction ("Aim", IE_Pressed, this, &AShooterCharacter::DoAim);
	PlayerInputComponent->BindAction ("Aim", IE_Released, this, &AShooterCharacter::DoAim);

	PlayerInputComponent->BindAction ("Fire", IE_Pressed, this, &AShooterCharacter::Shoot);

	PlayerInputComponent->BindAction ("DEBUG_ShootAtEnemies", IE_Pressed, this, &AShooterCharacter::DEBUG_ShootAtAllEnemies);

	//We can do this if we want to hold the crouch button - I want to have it toggled
	//PlayerInputComponent->BindAction ("Crouch", IE_Released, this, &AShooterCharacter::DoCrouth);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AShooterCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AShooterCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AShooterCharacter::OnResetVR);
}

void AShooterCharacter::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);

	SmoothRotate (DeltaTime, 6.0f);
	SmoothFollow (DeltaTime, 70.0f);

	UpdateHUDVars ();
}

FGenericTeamId AShooterCharacter::GetGenericTeamId () const
{
	return teamID;
}

bool AShooterCharacter::CanBeSeenFrom (const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	static const FName NAME_AILineOfSight = FName (TEXT ("TestPawnLineOfSight"));
	FHitResult hitResult;
	const bool bHit = GetWorld ()->LineTraceSingleByObjectType (hitResult, ObserverLocation, GetActorLocation (),
																FCollisionObjectQueryParams (ECC_TO_BITFIELD (ECC_WorldStatic) | ECC_TO_BITFIELD (ECC_WorldDynamic)),
																FCollisionQueryParams (NAME_AILineOfSight, true, IgnoreActor));
	NumberOfLoSChecksPerformed++;

	//If wanting to check for arms etc, could add hit boxes that cover major limbs (head, arms, body, legs) and iterate through them and checking if we hit it
	FVector socketLocation = GetMesh ()->GetSocketLocation ("neck_01");
	const bool bSocketHit = GetWorld ()->LineTraceSingleByObjectType (hitResult, ObserverLocation, socketLocation,
																	  FCollisionObjectQueryParams (ECC_TO_BITFIELD (ECC_WorldStatic) | ECC_TO_BITFIELD (ECC_WorldDynamic)), 
																	  FCollisionQueryParams (NAME_AILineOfSight, true, IgnoreActor));

	if (bSocketHit == false) {
		OutSeenLocation = GetActorLocation ();
		OutSightStrength = 1;
		return true;
	}

	if (bHit == false || (hitResult.GetActor ()->IsOwnedBy (this))) {
		OutSeenLocation = GetActorLocation();
		OutSightStrength = 1;
		return true;
	}
	OutSightStrength = 0;
	return false;
}


void AShooterCharacter::OnResetVR()
{
	// If Shooter is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in Shooter.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AShooterCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AShooterCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AShooterCharacter::SmoothRotate (float DeltaTime, float interpSpeed)
{
	//If we are aiming we need to check if we aim past +/- 90 degrees and rotate our character if we do
	if (bAiming) {
		//Get the camera forward vector
		FVector cameraForward = FollowCamera->GetForwardVector ();

		//Get the actor forward vector
		FVector actorForward = GetActorForwardVector ();

		//Get the dot product of the two vectors
		float dotProd = FVector::DotProduct (cameraForward, actorForward);

		if (dotProd < 0) {
			//Rotate actor forward vector to face camera forward vector with interpolation
			FVector rotationVector = FMath::VInterpConstantTo (actorForward, cameraForward, DeltaTime, interpSpeed);

			//Apply rotation only on the yaw axis
			SetActorRotation (FRotator (0, rotationVector.Rotation ().Yaw, 0));
		}
	}
	//Get the speed by velocity size
	float speed = GetVelocity ().Size ();

	//If speed is greater than 0 (i.e. we are moving)
	if (speed > 0)
	{
		//Get the camera forward vector
		FVector cameraForward = FollowCamera->GetForwardVector ();

		//Get the actor forward vector
		FVector actorForward = GetActorForwardVector ();

		//Rotate actor forward vector to face camera forward vector with interpolation
		FVector rotationVector = FMath::VInterpConstantTo (actorForward, cameraForward, DeltaTime, interpSpeed);

		//Apply rotation only on the yaw axis
		SetActorRotation (FRotator (0, rotationVector.Rotation ().Yaw, 0));
	}
}

void AShooterCharacter::SmoothFollow (float DeltaTime, float interpSpeed)
{
	//Check if wer are aiming first
	if (bAiming) {
		float currentLength = CameraBoom->TargetArmLength;
		float newLength = 300.0f;

		//Interpolate 5 times faster than usual
		float distance = FMath::FInterpConstantTo (currentLength, newLength, DeltaTime, interpSpeed * 5.f);

		CameraBoom->TargetArmLength = distance;

		return;
	}
	//Get the speed by velocity size
	float speed = GetVelocity ().Size ();

	//If we are moving
	if (speed > 0) {
		//Move our camera out (lerp Boom arm length)
		float currentLength = CameraBoom->TargetArmLength;
		float newLength = 450.0f;

		float distance = FMath::FInterpConstantTo (currentLength, newLength, DeltaTime, interpSpeed);

		CameraBoom->TargetArmLength = distance;
	}
	//We are not moving
	else {
		//Move our camera in
		float currentLength = CameraBoom->TargetArmLength;
		float newLength = 350.0f;

		float distance = FMath::FInterpConstantTo (currentLength, newLength, DeltaTime, interpSpeed);

		CameraBoom->TargetArmLength = distance;
	}
}

void AShooterCharacter::DoCrouch ()
{
	//Check if we are crouching and do the opposite
	GetCharacterMovement ()->IsCrouching () ? UnCrouch () : Crouch ();
}

void AShooterCharacter::DoAim ()
{
	bAiming ? AimOut () : AimIn ();
}

void AShooterCharacter::AimIn ()
{
	bAiming = true;
	//bUseControllerRotationYaw = true;
	GetCharacterMovement ()->MaxWalkSpeed *= .6f;
	GetCharacterMovement ()->MaxWalkSpeedCrouched *= .6f;
}

void AShooterCharacter::AimOut ()
{
	bAiming = false;
	//bUseControllerRotationYaw = false;
	GetCharacterMovement ()->MaxWalkSpeed = 600.f;
	GetCharacterMovement ()->MaxWalkSpeedCrouched = 300.f;

}

void AShooterCharacter::Shoot ()
{
	if (!bAiming) {
		//Hip Fire
	}
	MuzzleOffset.X = CameraBoom->TargetArmLength;
	FVector pos = FollowCamera->GetComponentLocation () + FTransform(FollowCamera->GetForwardVector().Rotation()).TransformVector(MuzzleOffset);
	FRotator rot = FollowCamera->GetForwardVector ().Rotation ();
	FTransform trans = FTransform (rot, pos, FVector (.2f, .2f, .2f));
	FActorSpawnParameters spawnInfo;
	GetWorld ()->SpawnActor<ABullet>(BulletBP, trans, spawnInfo);

}

void AShooterCharacter::UpdateHUDVars ()
{
	ACharacterHUD* HUD = (ACharacterHUD*) GetWorld ()->GetFirstPlayerController ()->GetHUD ();

	HUD->bAiming = bAiming;
}

void AShooterCharacter::DEBUG_ShootAtAllEnemies ()
{
	TArray<AActor*> enemies;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), AEnemyCharacter::StaticClass(), enemies);
	for (int i = 0; i < enemies.Num (); i++) {
		AEnemyCharacter* character = Cast<AEnemyCharacter> (enemies[i]);
		character->DEBUG_ShotAt ();
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
