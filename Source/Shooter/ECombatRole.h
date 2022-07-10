#pragma once
#include "CoreMinimal.h"

#include "ECombatRole.generated.h"

UENUM(BlueprintType)
enum class ECombatRole : uint8 {
	SHOOTER UMETA (DisplayName = "Shooter"),
	FLANKER UMETA (DisplayName = "Flanker"),
	APPROACHER UMETA (DisplayName = "Approacher"),
	SUPPORTER UMETA (DisplayName = "Supporter"),
	NONE UMETA (DisplayName = "No role")
};
