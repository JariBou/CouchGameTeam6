// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/GameModeBase.h>
#include <Systems/CharacterSelectionSubsystem.h>

#include "MenuSelectionMode.generated.h"

/**
 * 
 */
UCLASS()
class ROYALRIOT_API AMenuSelectionMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
	private:
	UCharacterSelectionSubsystem* CharacterSelectionSubsystem;
};
