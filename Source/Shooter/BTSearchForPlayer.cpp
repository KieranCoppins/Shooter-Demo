// Fill out your copyright notice in the Description page of Project Settings.


#include "BTSearchForPlayer.h"

EBTNodeResult::Type UBTSearchForPlayer::ExecuteTask (UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyController* EnemyController = Cast<AEnemyController> (OwnerComp.GetAIOwner ());

	if (EnemyController) {

		

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
