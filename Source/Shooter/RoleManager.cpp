// Fill out your copyright notice in the Description page of Project Settings.


#include "RoleManager.h"

// Sets default values
ARoleManager::ARoleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	flankTime = 30.f;
	approachTime = 30.f;

	roleAssignment = { {ECombatRole::APPROACHER, nullptr},
					  {ECombatRole::FLANKER, nullptr},
					  {ECombatRole::SHOOTER, nullptr},
					  {ECombatRole::SUPPORTER, nullptr} };

}

// Called when the game starts or when spawned
void ARoleManager::BeginPlay()
{
	Super::BeginPlay();

	// Get all the characters who are in the scene
	TArray<AActor*> AIActors;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), AEnemyCharacter::StaticClass (), AIActors);

	//Cast all of them as enemy characters and save
	for (int i = 0; i < AIActors.Num (); i++) {
		AEnemyCharacter* ai = Cast<AEnemyCharacter> (AIActors[i]);
		AIAgents.push_back (ai);
		Cast<AEnemyController> (ai->GetController ())->SetCombatRole (ECombatRole::NONE);		
	}
}

void ARoleManager::AssignRole (AEnemyCharacter* ai, ECombatRole role)
{
	//Make sure the role is not taken
	if (!RequestRole (role))
		return;

	roleAssignment[role] = ai;
	//ai->SetRole (role);
	Cast<AEnemyController> (ai->GetController ())->SetCombatRole (role);

}

void ARoleManager::ClearRole (ECombatRole role)
{
	//roleAssignment[role]->SetRole (ECombatRole::NONE);

	// check if the roles free before clearing it
	if (RequestRole(role))
		return;
	Cast<AEnemyController> (roleAssignment[role]->GetController ())->SetCombatRole (ECombatRole::NONE);
	roleAssignment[role] = nullptr;
}

AEnemyCharacter* ARoleManager::Choose (ECombatRole role)
{
	AEnemyCharacter* suitableAI = nullptr;

	// The approacher should be someone who:
	// - Has not got a role
	// - Is the closest to the last known position
	//The supporter should be someone who:
	// - Has not got a role
	// - Has line of sight to the last known position area
	// - Is closest after approacher is chosen
	if (role == ECombatRole::APPROACHER || role == ECombatRole::SUPPORTER) {
		float smallestDistance = std::numeric_limits<float>::infinity();
		for (int i = 0; i < AIAgents.size(); i++) {
			AEnemyController* controller = Cast<AEnemyController> (AIAgents[i]->GetController ());
			FVector pos = AIAgents[i]->GetActorLocation ();
			FVector lastKnownPos = controller->GetLastPosition ();
			float distance = FVector::Dist (pos, lastKnownPos);
			if (distance < smallestDistance && controller->GetCombatRole() == (uint8)ECombatRole::NONE) {
				smallestDistance = distance;
				suitableAI = AIAgents[i];
			}
		}
	}

	//The flanker should be someone who:
	// - Has not got a role
	// - Has the most suitable path to stay out of sight to flank the player (in TLOU this is done by using a bit map of where the player can see)
	else if (role == ECombatRole::FLANKER) {
		float smallestDotProduct = std::numeric_limits<float>::infinity ();
		for (int i = 0; i < AIAgents.size (); i++) {
			// TEMP: use the dot product to determine who has is furthest from the players peripheral vision

			// Get our enemy controller
			AEnemyController* EnemyController = Cast<AEnemyController> (AIAgents[i]->GetController());

			// Calculate the vector between our position and the last known position
			FVector vectorA = AIAgents[i]->GetActorLocation () - EnemyController->GetLastPosition ();
			vectorA.Normalize ();

			// Get the forward vector from the last known rotation the player was looking
			FVector lastKnownForwardVector = UKismetMathLibrary::GetForwardVector (EnemyController->GetLastRotation ());

			//Calculate the dot product
			float dotProduct = FVector::DotProduct (vectorA, lastKnownForwardVector * -1);

			if (dotProduct < smallestDotProduct && EnemyController->GetCombatRole() == (uint8) ECombatRole::NONE) {
				smallestDotProduct = dotProduct;
				suitableAI = AIAgents[i];
			}
		}
	}
	return suitableAI;
}

// Called every frame
void ARoleManager::Tick(float DeltaTime)
{
	Super::Tick (DeltaTime);

	// Check if we have a shooter
	if (!RequestRole (ECombatRole::SHOOTER)) {
		// If the shooter doesn't have line of sight
		if (!roleAssignment[ECombatRole::SHOOTER]->HasLineOfSight ()) {
			// They should no longer be the shooter
			ClearRole (ECombatRole::SHOOTER);
		}
	}

	// Check every frame if someone can see the player
	if (RequestRole (ECombatRole::SHOOTER)) {
		for (int i = 0; i < AIAgents.size (); i++) {
			if (AIAgents[i]->HasLineOfSight ()) {
				UE_LOG (LogTemp, Warning, TEXT ("Assigning a Shooter"));
				AssignRole (AIAgents[i], ECombatRole::SHOOTER);
				break;
			}
		}
	}
	// If combat time > threshold we should pick a flanker
	AEnemyController* EnemyController = Cast<AEnemyController> (AIAgents[0]->GetController ());

	if (EnemyController->GetModeDuration () > flankTime) {
		if (RequestRole (ECombatRole::FLANKER)) {
			UE_LOG (LogTemp, Warning, TEXT ("Assigning a Flanker"));
			AEnemyCharacter* ai = Choose (ECombatRole::FLANKER);
			AssignRole (ai, ECombatRole::FLANKER);
		}
	}

	// If last known position age is > threshold we should pick a approacher and a supporter
	if (EnemyController->GetLastKnownAge () > approachTime) {
		if (RequestRole (ECombatRole::APPROACHER)) {
			UE_LOG (LogTemp, Warning, TEXT ("Assigning a APPROACHER"));
			AEnemyCharacter* ai = Choose (ECombatRole::APPROACHER);
			if (ai != nullptr) {
				AssignRole (ai, ECombatRole::APPROACHER);
			}
		}

		if (RequestRole (ECombatRole::SUPPORTER)) {
			UE_LOG (LogTemp, Warning, TEXT ("Assigning a SUPPORTER"));
			AEnemyCharacter* ai = Choose (ECombatRole::SUPPORTER);
			if (ai != nullptr) {
				AssignRole (ai, ECombatRole::SUPPORTER);
			}
		}
	}
	// If last known position age is < threshold we should revoke approacher and supporter roles
	else {
		ClearRole (ECombatRole::APPROACHER);
		ClearRole (ECombatRole::SUPPORTER);
	}

}

bool ARoleManager::RequestRole (ECombatRole role)
{
	return roleAssignment[role] == nullptr;
}

