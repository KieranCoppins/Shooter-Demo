// Fill out your copyright notice in the Description page of Project Settings.


#include "BTShootPlayer.h"

EBTNodeResult::Type UBTShootPlayer::ExecuteTask (UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyController* EnemyController = Cast<AEnemyController> (OwnerComp.GetAIOwner ());

	if (EnemyController) {
		AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter> (EnemyController->GetCharacter ());

		//Check if we are taking damage / getting shot at
		if (EnemyController->bShotAt) {
			EnemyCharacter->Crouch ();
			EnemyController->fTimeSinceShotAt = 0.f;
		}

		if (EnemyController->fTimeSinceShotAt > 2.5f) {
			EnemyCharacter->UnCrouch ();


			if (EnemyController->GetLastKnownAge () == 0.f) {
				//We are looking at the target, we should shoot them
			}
		}


		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
