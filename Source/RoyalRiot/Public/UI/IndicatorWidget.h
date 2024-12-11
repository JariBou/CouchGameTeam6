// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/UserWidget.h>
#include "IndicatorWidget.generated.h"

UENUM(BlueprintType)
enum EIndicatorDisplayAction
{
	ShowIndicator,
	HideIndicator,
	DoNothing
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ROYALRIOT_API UIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitWithValue(float Value);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateValue(float Value);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowIndicator();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideIndicator();
};
