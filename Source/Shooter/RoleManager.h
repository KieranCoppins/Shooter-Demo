// Fill out your copyright notice in the Description page of Project Settings.

// The role manager needs
// - References to all AI in the scene
// - The current combat state
// The role manager decides
// - Which AI get what role
// - Permits roles when requested

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ECombatRole.h"
#include "EnemyCharacter.h"

#include <map>
#include <limits>

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "RoleManager.generated.h"

UCLASS()
class SHOOTER_API ARoleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoleManager();

	// Called every frame
	virtual void Tick (float DeltaTime) override;

	// Returns TRUE if role is available
	bool RequestRole (ECombatRole role);

	// Assign a role to an AI
	void AssignRole (AEnemyCharacter* ai, ECombatRole role);

	UPROPERTY (EditAnywhere)
		float flankTime;

	UPROPERTY (EditAnywhere)
		float approachTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	// Stores each role as a key, stores the character that has that role, if available stores nullptr
	std::map<ECombatRole, AEnemyCharacter*> roleAssignment;

	// Stores all AIs in the scene
	std::vector<AEnemyCharacter*> AIAgents;

	// Clears the passed role
	void ClearRole (ECombatRole role);

	// Whos the best suited for role?
	AEnemyCharacter* Choose (ECombatRole role);
};
