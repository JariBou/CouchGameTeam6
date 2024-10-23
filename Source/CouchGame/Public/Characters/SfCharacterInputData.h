// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SfCharacterInputData.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS()
class COUCHGAME_API USfCharacterInputData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionMove;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionRun;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionPickUpAndThrow;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionKnightWeapon;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionKnightDodge;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionSquireInteract;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionSquireDash;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionSquireSlap;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionSquireTaunt;
};
