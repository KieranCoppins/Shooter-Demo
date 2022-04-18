#pragma once
#include "CoreMinimal.h"

#include "CombatModesEnum.generated.h"

UENUM(BlueprintType)
enum class ECombatModes : uint8 {
	PATROL UMETA(DisplayName = "Patrol"),
	ALERT UMETA(DisplayName = "Alert"),
	SEARCH UMETA(DisplayName = "Search"),
	COMBAT UMETA(DisplayName = "Combat")
};