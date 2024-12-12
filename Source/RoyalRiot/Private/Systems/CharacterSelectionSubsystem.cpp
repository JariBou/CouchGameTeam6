// Fill out your copyright notice in the Description page of Project Settings.

#include <Systems/CharacterSelectionSubsystem.h>
#include <Kismet/GameplayStatics.h>

// FPlayerSelectionInfo& UCharacterSelectionSubsystem::InitializePlayerSelectionInfo(APlayerController* PlayerController)
// {
// 	// int32 ControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
// 	// TODO: add if exists to ignore
// 	// TODO: USE DEVICE ID BECAUSE CONTROLLERS ARE DIFFERENT BETWEEN LEVELS
// 	FPlayerSelectionInfo Info = FPlayerSelectionInfo();
// 	uint8 InfoIndex = m_newControllerIndex;
// 	Info.ControllerId = InfoIndex;
// 	Info.PlayerController = PlayerController;
// 	
// 	m_playerInfoArray.Add(Info);
// 	m_newControllerIndex++;
// 	return m_playerInfoArray[InfoIndex];
// }

FPlayerSelectionInfo& UCharacterSelectionSubsystem::InitializePlayerSelectionInfoForId(uint8 ControllerId, TEnumAsByte<ETeam> Team)
{
	if (m_controllerIdToPlayerInfoMap.Contains(ControllerId)) return m_controllerIdToPlayerInfoMap[ControllerId];
	FPlayerSelectionInfo Info = FPlayerSelectionInfo();
	uint8 InfoIndex = ControllerId;
	Info.ControllerId = InfoIndex;
	Info.PlayerTeam = Team;
	m_controllerIdToPlayerInfoMap.Add(ControllerId, Info);
	return m_controllerIdToPlayerInfoMap[ControllerId];
}

// FPlayerSelectionInfo& UCharacterSelectionSubsystem::GetPlayerSelectionInfo(APlayerController* PlayerController)
// {
// 	for (FPlayerSelectionInfo& Info : m_playerInfoArray)
// 	{
// 		if (Info.PlayerController == PlayerController) return Info;
// 	}
// 	throw;
// 	int32 ControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
// 	return GetPlayerSelectionInfoFromId(ControllerID);
// }

FPlayerSelectionInfo& UCharacterSelectionSubsystem::GetPlayerSelectionInfoFromId(uint8 ControllerId)
{
	// APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), ControllerId);
	// return GetPlayerSelectionInfo(PlayerController);
	return m_controllerIdToPlayerInfoMap[ControllerId];
}

// void UCharacterSelectionSubsystem::ChangePlayerTeam(APlayerController* PlayerController, TEnumAsByte<ETeam> NewTeam)
// {
// 	int32 ControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
// 	GetPlayerSelectionInfo(PlayerController).PlayerTeam = NewTeam;
// }

void UCharacterSelectionSubsystem::ChangePlayerTeamFromId(uint8 ControllerId, TEnumAsByte<ETeam> NewTeam)
{
	// APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), ControllerId);
	// ChangePlayerTeam(PlayerController, NewTeam);
	m_controllerIdToPlayerInfoMap[ControllerId].PlayerTeam = NewTeam;
	
}
