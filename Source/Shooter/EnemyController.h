// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AIController.h"

#include "CombatModesEnum.h"
#include "ECombatActions.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"

#include "EnemyController.generated.h"


/**
 * 
 */
UCLASS()
class SHOOTER_API AEnemyController : public AAIController
{
	GENERATED_BODY ()

	//Our Behaviour Tree
	UBehaviorTreeComponent* BehaviorComp;

	//Our Blackboard Component
	UBlackboardComponent* BlackboardComp;

	UPROPERTY (VisibleAnywhere, Category = AI)
		class UAIPerceptionComponent* PerceptionComp;

	UAISenseConfig_Sight* sightConfig;

	virtual void OnPossess (APawn* pawn) override;

	UFUNCTION()
		void OnSense (const TArray<AActor*>& testActors);

public:
	AEnemyController ();

	//Blackboard Keys
	UPROPERTY (EditDefaultsOnly, Category = AI)
		FName LocationToGoKey;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		FName CombatModeKey;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		FName LastPosKey;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		FName LastRotKey;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		FName LastKnownAgeKey;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		FName TargetKey;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		FName CombatActionKey;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		FName SmallestDotProductKey;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		FName MinPDKey;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		FName MaxPDKey;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		FName CoverPositionKey;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		float SearchRadius;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		float CombatDuration;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		float SearchDuration;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		float PlayerAngleThreshold;

	UPROPERTY (EditDefaultsOnly, Category = AI)
		float PlayerCoverDuration;

	bool bInCover;
	bool bShotAt;
	float fTimeSinceShotAt;

	void SetCombatMode (ECombatModes cm);
	void SetCombatAction (ECombatActions ca);
	void SetLastPosition (FVector pos);
	void SetLastRotation (FRotator rot);
	void SetLastKnownAge (float age);
	void SetTarget (APawn* pawn);
	void SetTarget (AActor* actor);
	void SetSmallestDotProduct (float dotProduct);

	void UpdateLastKnownAge (float DeltaTime);

	void UpdateCombatMode ();

	FORCEINLINE UBlackboardComponent* GetBlackboardComp () const { return BlackboardComp; }

	FORCEINLINE AActor* GetTarget () const { return Cast<AActor>(BlackboardComp->GetValueAsObject (TargetKey)); }

	UFUNCTION(BlueprintPure) FORCEINLINE
		FVector GetLastPosition () const { return BlackboardComp->GetValueAsVector (LastPosKey); }

	UFUNCTION(BlueprintPure) FORCEINLINE
		FVector GetCoverPosition () const { return BlackboardComp->GetValueAsVector (CoverPositionKey); }



	FORCEINLINE FRotator GetLastRotation () const { return BlackboardComp->GetValueAsRotator (LastRotKey); }

	FORCEINLINE uint8 GetCombatMode () const { return BlackboardComp->GetValueAsEnum (CombatModeKey); }
	FORCEINLINE uint8 GetCombatAction () const { return BlackboardComp->GetValueAsEnum (CombatActionKey); }

	FORCEINLINE float GetSmallestDotProduct () const { return BlackboardComp->GetValueAsFloat (SmallestDotProductKey); }
	FORCEINLINE float GetLastKnownAge () const { return BlackboardComp->GetValueAsFloat (LastKnownAgeKey); }
	FORCEINLINE float GetMinPD () const { return BlackboardComp->GetValueAsFloat (MinPDKey); }
	FORCEINLINE float GetMaxPD () const { return BlackboardComp->GetValueAsFloat (MaxPDKey); }

	int32 currentWaypoint;
	
};
