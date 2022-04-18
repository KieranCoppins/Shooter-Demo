// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "EnemyCharacter.h"
#include "EnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "BTAttackPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UBTAttackPlayer : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask (UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
