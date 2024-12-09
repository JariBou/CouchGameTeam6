// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MenuInputData.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS()
class ROYALRIOT_API UMenuInputData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> Navigate;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> Confirm;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> Back;
};
