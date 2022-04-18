// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "EnemyController.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent> (TEXT("Pawn Sensing Component"));
	PawnSensingComp->SetPeripheralVisionAngle (90.0f);

	baseMovementSpeed = GetCharacterMovement ()->MaxWalkSpeed;
}

void AEnemyCharacter::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);

	AEnemyController* enemyController = Cast<AEnemyController> (GetController());
	enemyController->UpdateCombatMode ();
	enemyController->UpdateLastKnownAge (DeltaTime);

	enemyController->fTimeSinceShotAt += DeltaTime;

	//Check our distance to our cover position, if we're close enough we can say we're in cover
	float distToCover = FVector::Distance (GetActorLocation (), enemyController->GetCoverPosition());
	if (distToCover <= 50.f) {
		enemyController->bInCover = true;
	}
	else {
		enemyController->bInCover = false;
	}

	if (enemyController->GetCombatMode () == (uint8)ECombatModes::COMBAT) {
		GetCharacterMovement ()->MaxWalkSpeed = baseMovementSpeed;
	}
	else {
		GetCharacterMovement ()->MaxWalkSpeed = baseMovementSpeed / 3;
	}
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::DEBUG_ShotAt ()
{
	UE_LOG (LogTemp, Warning, TEXT ("DEBUG_ShotAt"));
	AEnemyController* enemyController = Cast<AEnemyController> (GetController ());
	enemyController->bShotAt ? enemyController->bShotAt = false : enemyController->bShotAt = true;
}

