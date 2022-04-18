

#include "EnemyController.h"

#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

void AEnemyController::OnPossess (APawn* pawn)
{
	Super::OnPossess (pawn);

	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter> (pawn);

	if (EnemyCharacter) {
		if (EnemyCharacter->BehaviourTree->BlackboardAsset) {
			BlackboardComp->InitializeBlackboard (*(EnemyCharacter->BehaviourTree->BlackboardAsset));
		}

		BehaviorComp->StartTree (*EnemyCharacter->BehaviourTree);
	}

	if (PerceptionComp) {
		PerceptionComp->OnPerceptionUpdated.AddDynamic (this, &AEnemyController::OnSense);
		UAIPerceptionSystem::RegisterPerceptionStimuliSource (this, sightConfig->GetSenseImplementation (), pawn);
	}

	//Blackboard default values
	BlackboardComp->SetValueAsFloat (MinPDKey, 500.f);
	BlackboardComp->SetValueAsFloat (MaxPDKey, 1200.f);
}

void AEnemyController::OnSense (const TArray<AActor*>& testActors)
{
	SetTarget (testActors[0]);
}

AEnemyController::AEnemyController ()
{

	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent> (TEXT ("Behavior Tree"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent> (TEXT ("Blackboard"));

	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent> (TEXT ("AI Perception"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight> (TEXT ("Sight Config"));

	PerceptionComp->ConfigureSense (*sightConfig);
	PerceptionComp->SetDominantSense (sightConfig->GetSenseImplementation ());

	SetGenericTeamId (FGenericTeamId (1));

	LocationToGoKey = "LocationToGo";
	CombatModeKey = "CombatMode";
	LastPosKey = "LastKnownPosition";
	LastRotKey = "LastKnownRotation";
	LastKnownAgeKey = "LastKnownPositionAge";
	TargetKey = "Target";
	CombatActionKey = "CombatAction";
	SmallestDotProductKey = "SmallestDotProduct";
	MinPDKey = "MinDistanceFromPlayer";
	MaxPDKey = "MaxDistanceFromPlayer";
	CoverPositionKey = "CoverPosition";

	SearchDuration = 120.f;
	CombatDuration = 120.f;
	PlayerAngleThreshold = -0.5f;
	PlayerCoverDuration = 25.f;

	bInCover = false;
	bShotAt = false;

	fTimeSinceShotAt = 0.f;

	currentWaypoint = 0;

}

void AEnemyController::SetCombatMode (ECombatModes cm)
{
	if (BlackboardComp) {
		if (BlackboardComp->GetValueAsEnum (CombatModeKey) != (uint8) cm){
			BlackboardComp->SetValueAsEnum (CombatModeKey, (uint8) cm);
			BehaviorComp->RestartTree ();
		}
	}
}

void AEnemyController::SetCombatAction (ECombatActions ca)
{
	if (BlackboardComp) {
		if (BlackboardComp->GetValueAsEnum (CombatActionKey) != (uint8) ca) {
			BlackboardComp->SetValueAsEnum (CombatActionKey, (uint8) ca);
		}
	}
}

void AEnemyController::SetLastPosition (FVector pos)
{
	if (BlackboardComp)
	{
		FVector prevPosition = BlackboardComp->GetValueAsVector (LastPosKey);
		float distance = FVector::Distance (prevPosition, pos);
		if (distance > 100.0f) {
			SetLastKnownAge (0);
			BlackboardComp->SetValueAsVector (LastPosKey, pos);
		}
	}
}

void AEnemyController::SetLastRotation (FRotator rot)
{
	if (BlackboardComp)
		BlackboardComp->SetValueAsRotator (LastRotKey, rot);
}

void AEnemyController::SetLastKnownAge (float age)
{
	if (BlackboardComp)
		BlackboardComp->SetValueAsFloat (LastKnownAgeKey, age);
}

void AEnemyController::SetTarget (APawn* pawn)
{
	if(BlackboardComp)
		BlackboardComp->SetValueAsObject(TargetKey, pawn);
}

void AEnemyController::SetTarget (AActor* actor)
{
	if (BlackboardComp)
		BlackboardComp->SetValueAsObject (TargetKey, actor);
}

void AEnemyController::SetSmallestDotProduct (float dotProduct)
{
	if (BlackboardComp)
		BlackboardComp->SetValueAsEnum (SmallestDotProductKey, dotProduct);
}

void AEnemyController::UpdateLastKnownAge (float DeltaTime)
{
	if (BlackboardComp) {
		SetLastKnownAge (GetLastKnownAge () + DeltaTime);
	}
}

//This could be promoted to a service so its all controlled in the behaviour tree
void AEnemyController::UpdateCombatMode ()
{
	FActorPerceptionBlueprintInfo info;
	if (PerceptionComp->GetActorsPerception (UGameplayStatics::GetPlayerPawn (GetWorld (), 0), info)) {
		FAIStimulus stimulus = info.LastSensedStimuli[0];
		//Keep following location after 1 second - could utilise the sense prediction
		if (stimulus.GetAge () < 1.f) {
			if (BlackboardComp) {
				SetLastPosition (GetTarget ()->GetActorLocation ());
				SetLastRotation (GetTarget ()->GetActorRotation ());
			}
		}

		//If our age is less than our combat duration we should stay in combat
		if (stimulus.GetAge () <= CombatDuration) {
			SetCombatMode (ECombatModes::COMBAT);
		}
		//If we're here then we're over the combat duration and need to be between combat duration and combat + search duration
		else if (stimulus.GetAge () <= CombatDuration + SearchDuration) {
			SetCombatMode (ECombatModes::SEARCH);
		}
		//If all durations are exhausted, go back to patrolling
		else {
			SetCombatMode (ECombatModes::PATROL);
		}
	}
}
