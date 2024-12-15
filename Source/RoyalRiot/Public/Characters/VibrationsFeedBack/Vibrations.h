// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Vibrations.generated.h"

/**
 * 
 */

USTRUCT()
struct FVibrationsStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UForceFeedbackEffect> ForceFeedbackEffect;

	UPROPERTY(EditAnywhere)
	FName Tag;
};

UCLASS()
class ROYALRIOT_API UVibrations : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FVibrationsStruct Mud;

	UPROPERTY(EditAnywhere)
	FVibrationsStruct KnightReceive;

	UPROPERTY(EditAnywhere)
	FVibrationsStruct SquireGive;

	UPROPERTY(EditAnywhere)
	FVibrationsStruct HitOpponent;

	UPROPERTY(EditAnywhere)
	FVibrationsStruct TakeDamage;

	UPROPERTY(EditAnywhere)
	FVibrationsStruct Death;

	UPROPERTY(EditAnywhere)
	FVibrationsStruct KillOpponent;

	UPROPERTY(EditAnywhere)
	FVibrationsStruct Respawn;
};
