// Fill out your copyright notice in the Description page of Project Settings.


#include "BTAttackPlayer.h"

EBTNodeResult::Type UBTAttackPlayer::ExecuteTask (UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyController* EnemyController = Cast<AEnemyController> (OwnerComp.GetAIOwner ());

	if (EnemyController) {
		bool flank = false;
		//Provide logic for when the take cover, shoot the player, change cover, push, flank

		//Calculate the dot product between the player and the enemy
		AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(EnemyController->GetCharacter ());
		if (EnemyCharacter) {
			FVector vectorA = EnemyCharacter->GetActorLocation () - EnemyController->GetLastPosition ();
			vectorA.Normalize ();
			FVector lastKnownForwardVector = UKismetMathLibrary::GetForwardVector (EnemyController->GetLastRotation ());
			double dotProduct = FVector::DotProduct (vectorA, lastKnownForwardVector * -1);
			//UE_LOG (LogTemp, Warning, TEXT ("Last Known Age: %f"), EnemyController->GetLastKnownAge());
			if (dotProduct < EnemyController->PlayerAngleThreshold && EnemyController->bShotAt == false) {
				flank = true;
			}

			//Calculate distance from player
			float distance = FVector::Distance (EnemyCharacter->GetActorLocation (), EnemyController->GetLastPosition ());
			//UE_LOG (LogTemp, Warning, TEXT("Distance: %f"), distance);

			FHitResult hitResult;
			const bool bHit = GetWorld ()->LineTraceSingleByObjectType (hitResult, EnemyController->GetCoverPosition(), EnemyController->GetLastPosition() + FVector(0.f, 0.f, 40.f),
																		FCollisionObjectQueryParams (ECC_TO_BITFIELD (ECC_WorldStatic) | ECC_TO_BITFIELD (ECC_WorldDynamic)),
																		FCollisionQueryParams (FName(TEXT("CoverTrace")), true));

			if (bHit == false) {
				EnemyController->SetCombatAction (ECombatActions::MOVETOCOVER);
				EnemyCharacter->UnCrouch ();
				return EBTNodeResult::Succeeded;
			}

			if (flank && EnemyController->GetLastKnownAge () > EnemyController->PlayerCoverDuration) {
				EnemyController->SetCombatAction (ECombatActions::FLANK);
				EnemyCharacter->UnCrouch ();
			}
			else if (distance > EnemyController->GetMaxPD ()) {
				EnemyController->SetCombatAction (ECombatActions::PUSHFORWARD);
				EnemyCharacter->UnCrouch ();
			}
			else if (distance < EnemyController->GetMinPD ()) {
				EnemyController->SetCombatAction (ECombatActions::RETREAT);
				EnemyCharacter->UnCrouch ();
			}
			else if (!EnemyController->bInCover) {
				EnemyController->SetCombatAction (ECombatActions::MOVETOCOVER);
				EnemyCharacter->UnCrouch ();
			}
			else {
				EnemyController->SetCombatAction (ECombatActions::STAY);
			}


			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
