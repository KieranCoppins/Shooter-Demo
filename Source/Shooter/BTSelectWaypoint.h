// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "EnemyWaypoint.h"
#include "EnemyController.h"
#include "EnemyCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "BTSelectWaypoint.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UBTSelectWaypoint : public UBTTaskNode
{
	GENERATED_BODY ()

	virtual EBTNodeResult::Type ExecuteTask (UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
