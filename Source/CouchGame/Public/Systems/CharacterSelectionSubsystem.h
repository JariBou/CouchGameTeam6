// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Teams.h>

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include "CharacterSelectionSubsystem.generated.h"

USTRUCT()
struct FPlayerSelectionInfo
{
	GENERATED_BODY()

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	TEnumAsByte<ETeam> PlayerTeam; 
	
	
};

/**
 * 
 */
UCLASS()
class COUCHGAME_API UCharacterSelectionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	

	private:
	UPROPERTY(EditAnywhere)
	TMap<uint8_t, FPlayerSelectionInfo> m_playerIdToInfoMap;
};
