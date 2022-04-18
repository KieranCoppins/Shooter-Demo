// Fill out your copyright notice in the Description page of Project Settings.


#include "BTSelectWaypoint.h"

EBTNodeResult::Type UBTSelectWaypoint::ExecuteTask (UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyController* EnemyController = Cast<AEnemyController> (OwnerComp.GetAIOwner ());

	if (EnemyController) {
		UBlackboardComponent* BlackboardComp = EnemyController->GetBlackboardComp ();

		AEnemyWaypoint* currentWaypoint = Cast<AEnemyWaypoint> (BlackboardComp->GetValueAsObject (EnemyController->LocationToGoKey));

		AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(EnemyController->GetCharacter ());

		TArray<AEnemyWaypoint*> availableWaypoints = EnemyCharacter->Waypoints;

		AEnemyWaypoint* nextWaypoint = nullptr;

		if (EnemyController->currentWaypoint != availableWaypoints.Num () - 1) {
			nextWaypoint = Cast<AEnemyWaypoint> (availableWaypoints[++EnemyController->currentWaypoint]);
		}
		else {
			nextWaypoint = Cast<AEnemyWaypoint> (availableWaypoints[0]);
			EnemyController->currentWaypoint = 0;
		}

		BlackboardComp->SetValueAsObject (EnemyController->LocationToGoKey, nextWaypoint);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
