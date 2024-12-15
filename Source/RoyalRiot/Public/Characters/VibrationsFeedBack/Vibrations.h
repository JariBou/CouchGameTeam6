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
};
