// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/UserWidget.h>
#include "IndicatorWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ROYALRIOT_API UIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateValue(float Value);
};
