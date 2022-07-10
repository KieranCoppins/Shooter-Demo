#pragma once
#include "CoreMinimal.h"

#include "ECombatActions.generated.h"

UENUM(BlueprintType)
enum class ECombatActions : uint8 {
	MOVETOCOVER UMETA(DisplayName = "Move To Cover"),
	PUSHFORWARD UMETA(DisplayName = "Push Forward"),
	RETREAT UMETA(DisplayName = "Retreat"),
	FLANK UMETA(DisplayName = "Flank"),
	STAY UMETA(DisplayName = "Stay in Place"),
	SHOOT UMETA (DisplayName = "Shoot"),
	INVESTIGATE UMETA (DisplayName = "Investigate")
};