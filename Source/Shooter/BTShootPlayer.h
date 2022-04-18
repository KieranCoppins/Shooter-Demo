// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "EnemyCharacter.h"
#include "EnemyController.h"

#include "BTShootPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UBTShootPlayer : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask (UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
