// Fill out your copyright notice in the Description page of Project Settings.

#include <Systems/CharacterSelectionSubsystem.h>
#include <Kismet/GameplayStatics.h>

FPlayerSelectionInfo& UCharacterSelectionSubsystem::InitializePlayerSelectionInfo(APlayerController* PlayerController)
{
	int32 ControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
	FPlayerSelectionInfo Info = FPlayerSelectionInfo();
	Info.Id = ControllerID;
	Info.PlayerController = PlayerController;
	m_playerIdToInfoMap.Add(ControllerID, Info);
	return m_playerIdToInfoMap[ControllerID];
}

FPlayerSelectionInfo& UCharacterSelectionSubsystem::InitializePlayerSelectionInfoForId(uint8 PlayerId)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), PlayerId);
	return InitializePlayerSelectionInfo(PlayerController);
}

const FPlayerSelectionInfo& UCharacterSelectionSubsystem::GetPlayerSelectionInfo(APlayerController* PlayerController)
{
	int32 ControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
	return GetPlayerSelectionInfo(ControllerID);
}

const FPlayerSelectionInfo& UCharacterSelectionSubsystem::GetPlayerSelectionInfo(uint8 PlayerId)
{
	return m_playerIdToInfoMap[PlayerId];
}

void UCharacterSelectionSubsystem::ChangePlayerTeam(APlayerController* PlayerController, TEnumAsByte<ETeam> NewTeam)
{
	int32 ControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
	ChangePlayerTeam(ControllerID, NewTeam);
}

void UCharacterSelectionSubsystem::ChangePlayerTeam(uint8 PlayerId, TEnumAsByte<ETeam> NewTeam)
{
	m_playerIdToInfoMap[PlayerId].PlayerTeam = NewTeam;
}
