// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SfCharacterInputData.generated.h"

class USfCharacterState;
enum class ESfCharacterStateID : uint8;
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
	TObjectPtr<UInputAction> InputActionLeftJoystick;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionLeftJoystickButton;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionRightJoystick;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionFaceButtonUp;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionFaceButtonRight;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionFaceButtonLeft;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionFaceButtonDown;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionRightTrigger;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionLeftTrigger;

	UPROPERTY(EditAnywhere)
	TMap<ESfCharacterStateID, TSubclassOf<USfCharacterState>> CharacterStates;
};
