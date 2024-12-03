// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Teams.h>

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include "CharacterSelectionSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSelectionInfo
{
	GENERATED_BODY()

	// Should be useless but just in case
	UPROPERTY()
	uint8 Id;

	UPROPERTY()
	APlayerController* PlayerController = nullptr;

	UPROPERTY()
	TEnumAsByte<ETeam> PlayerTeam = Team1; 
};

/**
 * 
 */
UCLASS()
class COUCHGAME_API UCharacterSelectionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	public:
	FPlayerSelectionInfo& InitializePlayerSelectionInfo(APlayerController* PlayerController);
	FPlayerSelectionInfo& InitializePlayerSelectionInfoForId(uint8 ControllerId);

	FPlayerSelectionInfo& GetPlayerSelectionInfo(APlayerController* PlayerController);
	FPlayerSelectionInfo& GetPlayerSelectionInfo(uint8 PlayerId);
	FPlayerSelectionInfo& GetPlayerSelectionInfoFromArray(uint8 ArrayIndex);
	
	void ChangePlayerTeam(APlayerController* PlayerController, TEnumAsByte<ETeam> NewTeam);
	void ChangePlayerTeam(uint8 PlayerId, TEnumAsByte<ETeam> NewTeam);

	private:
	UPROPERTY(EditAnywhere)
	TArray<FPlayerSelectionInfo> m_playerInfoArray;

	uint8 m_newControllerIndex = 0;
};
