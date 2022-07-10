// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "EnemyController.h"
#include "EnemyWaypoint.h"

#include "BehaviorTree/BehaviorTree.h"
#include "Perception/PawnSensingComponent.h"

#include "ECombatRole.h"

#include "EnemyCharacter.generated.h"

UCLASS()
class SHOOTER_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()


public:	
	// Sets default values for this character's properties
	AEnemyCharacter ();

	// Called when the game starts or when spawned
	virtual void BeginPlay () override;

	//Called every frame
	virtual void Tick (float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY (EditAnywhere, Category = AI)
		class UBehaviorTree* BehaviourTree;

	UPROPERTY (VisibleAnywhere, Category = AI)
		class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY (EditAnywhere, Category = AI)
		TArray<AEnemyWaypoint*> Waypoints;

	void DEBUG_ShotAt ();

	void SetRole (ECombatRole role);

	FORCEINLINE ECombatRole GetRole () const { return currentRole; }
	FORCEINLINE bool HasLineOfSight () const { return bHasLineOfSight; }

private:
	float baseMovementSpeed;

	ECombatRole currentRole;

	bool bHasLineOfSight;

};
