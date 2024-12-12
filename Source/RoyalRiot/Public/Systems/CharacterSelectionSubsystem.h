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

	UPROPERTY()
	uint8 ControllerId;

	UPROPERTY() // Should be obsolete lmao
	APlayerController* PlayerController = nullptr;

	UPROPERTY()
	TEnumAsByte<ETeam> PlayerTeam = Team1; 
};

/**
 * 
 */
UCLASS()
class ROYALRIOT_API UCharacterSelectionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	public:
	// UFUNCTION(meta=(DeprecatedFunction, DeprecationMessage="Function has been deprecated, Use version with controller ID"))
	// FPlayerSelectionInfo& InitializePlayerSelectionInfo(APlayerController* PlayerController);
	UFUNCTION(BlueprintCallable)
	FPlayerSelectionInfo& InitializePlayerSelectionInfoForId(uint8 ControllerId, TEnumAsByte<ETeam> Team = Team1);
	// UFUNCTION(meta=(DeprecatedFunction, DeprecationMessage="Function has been deprecated, Use version with controller ID"))
	// FPlayerSelectionInfo& GetPlayerSelectionInfo(APlayerController* PlayerController);
	UFUNCTION(BlueprintCallable)
	FPlayerSelectionInfo& GetPlayerSelectionInfoFromId(uint8 PlayerId);

	// UFUNCTION(meta=(DeprecatedFunction, DeprecationMessage="Function has been deprecated, Use version with controller ID"))
	// void ChangePlayerTeam(APlayerController* PlayerController, TEnumAsByte<ETeam> NewTeam);
	UFUNCTION(BlueprintCallable)
	void ChangePlayerTeamFromId(uint8 PlayerId, TEnumAsByte<ETeam> NewTeam);

	private:
	UPROPERTY()
	TMap<uint8, FPlayerSelectionInfo> m_controllerIdToPlayerInfoMap;

	uint8 m_newControllerIndex = 0;
};
