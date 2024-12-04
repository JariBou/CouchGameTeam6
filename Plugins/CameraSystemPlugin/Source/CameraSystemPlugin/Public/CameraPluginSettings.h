// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CameraPluginSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Camera Plugin Settings"))
class CAMERASYSTEMPLUGIN_API UCameraPluginSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, category = "Camera")
	float CameraZoomDistanceBetweenTargetsMin = 300.f;

	UPROPERTY(Config, EditAnywhere, category = "Camera")
	float CameraZoomDistanceBetweenTargetsMax = 1500.f;
	
	UPROPERTY(Config, EditAnywhere, category = "Camera")
	float CameraRotationMaxOffsetAlphaX = .3f;

	UPROPERTY(Config, EditAnywhere, category = "Camera")
	float CameraRotationMaxOffsetAlphaY = .3f;

	UPROPERTY(Config, EditAnywhere, category = "Camera")
	float CameraRotationSpeedAlpha = .3f;
	
};
