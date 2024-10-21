// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerGameViewportClient.h"

#include "LocalMultiplayerSettings.h"
#include "LocalMultiplayerSubsystem.h"
#include "Kismet/GameplayStatics.h"

void ULocalMultiplayerGameViewportClient::PostInitProperties()
{
	LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();

	MaxSplitscreenPlayers = LocalMultiplayerSettings->KeyboardProfilesData.Num() + LocalMultiplayerSettings->nbMaxGamepads;
	Super::PostInitProperties();
}

bool ULocalMultiplayerGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	if (LMS == nullptr) LMS = GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();

	int PlayerIndex = -1;
	if (EventArgs.IsGamepad())
	{
		if (PlayerIndex = LMS->GetAssignedPlayerIndexFromGamepadDeviceID(EventArgs.ControllerId); PlayerIndex == -1)
		{
			PlayerIndex = LMS->AssignNewPlayerToGamepadDeviceID(EventArgs.ControllerId);
			LMS->AssignGamepadInputMapping(PlayerIndex, ELocalMultiplayerInputMappingType::InGame);
		} else
		{
		
		}
	} else
	{
		const int KeyboardProfileIndex = LocalMultiplayerSettings->FindKeyboardProfilesIndexFromKey(EventArgs.Key, ELocalMultiplayerInputMappingType::InGame);
		if (KeyboardProfileIndex != -1)
		{
			if (PlayerIndex = LMS->GetAssignedPlayerIndexFromKeyboardProfileIndex(KeyboardProfileIndex); PlayerIndex == -1)
			{
				PlayerIndex = LMS->AssignNewPlayerToKeyboardProfile(KeyboardProfileIndex);
				LMS->AssignKeyboardMapping(PlayerIndex, KeyboardProfileIndex, ELocalMultiplayerInputMappingType::InGame);
			} else
			{
		
			}
		}
		
	}

	if (PlayerIndex != -1)
	{
		return UGameplayStatics::GetPlayerController(GetGameInstance()->GetWorld(), PlayerIndex)->InputKey(EventArgs.Key,
			EventArgs.Event, EventArgs.AmountDepressed, EventArgs.IsGamepad());
	}

	return Super::InputKey(EventArgs);
}

bool ULocalMultiplayerGameViewportClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key,
	float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	if (LMS == nullptr) LMS = GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();

	if (!bGamepad) return false;
	
	int PlayerIndex;
	if (PlayerIndex = LMS->GetAssignedPlayerIndexFromGamepadDeviceID(InputDevice.GetId()); PlayerIndex == -1)
	{
		PlayerIndex = LMS->AssignNewPlayerToGamepadDeviceID(InputDevice.GetId());
		LMS->AssignGamepadInputMapping(PlayerIndex, ELocalMultiplayerInputMappingType::InGame);
	} else
	{
		
	}

	if (PlayerIndex != -1)
	{
		UGameplayStatics::GetPlayerController(GetGameInstance()->GetWorld(), PlayerIndex)->InputAxis(Key, Delta, DeltaTime, NumSamples, bGamepad);
	}
	
	return Super::InputAxis(Viewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
}
