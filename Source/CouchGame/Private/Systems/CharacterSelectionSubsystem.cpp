// Fill out your copyright notice in the Description page of Project Settings.

#include <Systems/CharacterSelectionSubsystem.h>
#include <Kismet/GameplayStatics.h>

FPlayerSelectionInfo& UCharacterSelectionSubsystem::InitializePlayerSelectionInfo(APlayerController* PlayerController)
{
	// int32 ControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
	FPlayerSelectionInfo Info = FPlayerSelectionInfo();
	uint8 InfoIndex = m_newControllerIndex;
	Info.Id = InfoIndex;
	Info.PlayerController = PlayerController;
	m_playerInfoArray.Add(Info);
	m_newControllerIndex++;
	return m_playerInfoArray[InfoIndex];
}

FPlayerSelectionInfo& UCharacterSelectionSubsystem::InitializePlayerSelectionInfoForId(uint8 ControllerId)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), ControllerId);
	return InitializePlayerSelectionInfo(PlayerController);
}

FPlayerSelectionInfo& UCharacterSelectionSubsystem::GetPlayerSelectionInfo(APlayerController* PlayerController)
{
	for (FPlayerSelectionInfo& Info : m_playerInfoArray)
	{
		if (Info.PlayerController == PlayerController) return Info;
	}
	throw;
	int32 ControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
	return GetPlayerSelectionInfo(ControllerID);
}

FPlayerSelectionInfo& UCharacterSelectionSubsystem::GetPlayerSelectionInfo(uint8 ControllerId)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), ControllerId);
	return GetPlayerSelectionInfo(PlayerController);
}

FPlayerSelectionInfo& UCharacterSelectionSubsystem::GetPlayerSelectionInfoFromArray(uint8 ArrayIndex)
{
	return m_playerInfoArray[ArrayIndex];
}

void UCharacterSelectionSubsystem::ChangePlayerTeam(APlayerController* PlayerController, TEnumAsByte<ETeam> NewTeam)
{
	int32 ControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
	GetPlayerSelectionInfo(PlayerController).PlayerTeam = NewTeam;
}

void UCharacterSelectionSubsystem::ChangePlayerTeam(uint8 ControllerId, TEnumAsByte<ETeam> NewTeam)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), ControllerId);
	ChangePlayerTeam(PlayerController, NewTeam);
}
