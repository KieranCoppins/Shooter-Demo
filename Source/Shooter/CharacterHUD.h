// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "Engine/Canvas.h"

#include "CharacterHUD.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API ACharacterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD () override;

	bool bAiming;

protected:
	//Crosshair Texture
	UPROPERTY (EditDefaultsOnly)
		UTexture2D* CrosshairTexture;

	
};
