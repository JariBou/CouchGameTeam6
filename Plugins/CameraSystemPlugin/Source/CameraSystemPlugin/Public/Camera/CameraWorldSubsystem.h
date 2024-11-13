// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CameraWorldSubsystem.generated.h"

class UCameraPluginSettings;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class CAMERASYSTEMPLUGIN_API UCameraWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

#pragma region Subsystem Overrides
public:
    virtual void  PostInitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override { return TStatId(); }

#pragma endregion

#pragma region Main Camera
protected:
	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraMain;

	UPROPERTY()
	const UCameraPluginSettings* CameraPluginSettings;

	void TickUpdateCameraZoom(float DeltaTime);
	
	void TickUpdateCameraRotation(float DeltaTime);
	
	UCameraComponent* FindCameraByTag(const FName& Tag) const;
	
#pragma endregion

#pragma region Follow Targets
public:

	void AddFollowTarget(UObject* FollowTarget);

	void RemoveFollowTarget(UObject* FollowTarget);

protected:
	UPROPERTY()
	TArray<UObject*> FollowTargets;
	
	FVector CalculateAveragePositionBetweenTargets();

	float CalculateGreatestYDistanceBetweenTargets();

#pragma endregion

#pragma region Rotation

protected:
	UPROPERTY()
	FVector StartForwardVector;
	
	UFUNCTION()
	void InitCameraRotationParameters();	
	
#pragma endregion

#pragma region Zoom
protected:
	// UPROPERTY()
	// float CameraZoomYMin = 0.f;

	UPROPERTY()
	FVector CameraZoomYMinLoc = FVector::Zero();

	// UPROPERTY()
	// float CameraZoomYMax = 0.f;

	UPROPERTY()
	FVector CameraZoomYMaxLoc = FVector::Zero();

	UPROPERTY()
	float CameraZoomDistanceBetweenTargetsMin = 300.f;

	UPROPERTY()
	float CameraZoomDistanceBetweenTargetsMax = 1500.f;

	UFUNCTION()
	void InitCameraZoomParameters();
	
	
#pragma endregion

#pragma region Bounds
protected:
	UPROPERTY()
	FVector2D CameraBoundsMin;

	UPROPERTY()
	FVector2D CameraBoundsMax;

	UPROPERTY()
	float CameraBoundsYProjectionCenter;

	AActor* FindCameraBoundsActor() const;

#pragma endregion
	
};
