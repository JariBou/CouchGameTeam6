// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerSubsystem.h"

#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "LocalMultiplayerSettings.h"

void ULocalMultiplayerSubsystem::CreateAndInitPlayers(ELocalMultiplayerInputMappingType MappingType)
{
	const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();

	// for (int i = 0; i < LocalMultiplayerSettings->KeyboardProfilesData.Num(); ++i)
	// {
	// 	APlayerController* PlayerController = UGameplayStatics::CreatePlayer(GetWorld(), i);
	// 	if (PlayerIndexFromKeyboardProfileIndex.Contains(i))
	// 	{
	// 		// Reassign doode
	// 		AssignKeyboardMapping(PlayerIndexFromKeyboardProfileIndex[i], i, MappingType);
	// 	}
	// 	else
	// 	{
	// 		
	// 	}
	// }

	for (int i = 0; i < LocalMultiplayerSettings->GetNbKeyboardProfiles() + LocalMultiplayerSettings->nbMaxGamepads; ++i)
	{
		APlayerController* PlayerController = UGameplayStatics::CreatePlayer(GetWorld(), i);
		if (PlayerIndexFromKeyboardProfileIndex.Contains(i))
		{
			// Reassign doode
			AssignKeyboardMapping(PlayerIndexFromKeyboardProfileIndex[i], i, MappingType);
		} else if (PlayerIndexFromKeyboardProfileIndex.FindKey(i))
		{
			// Reassign doode
			AssignGamepadInputMapping(i, MappingType);
		}
		else
		{
			
		}
	}
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromKeyboardProfileIndex(int KeyboardProfileIndex)
{
	const int* Find = PlayerIndexFromKeyboardProfileIndex.Find(KeyboardProfileIndex);
	return Find ? *Find : -1;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToKeyboardProfile(int KeyboardProfileIndex)
{
	int buff = LastAssignedPlayerIndex;
	PlayerIndexFromKeyboardProfileIndex.Add(KeyboardProfileIndex, LastAssignedPlayerIndex);
	LastAssignedPlayerIndex++;
	return buff;
}

void ULocalMultiplayerSubsystem::AssignKeyboardMapping(int PlayerIndex, int KeyboardProfileIndex,
	ELocalMultiplayerInputMappingType MappingType) const
{
	ULocalPlayer* LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(PlayerIndex);
	UEnhancedInputLocalPlayerSubsystem* PlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (PlayerSubsystem == nullptr) return;
	
	const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();

	TObjectPtr<UInputMappingContext> InputMappingContext = LocalMultiplayerSettings->KeyboardProfilesData[KeyboardProfileIndex].GetIMCFromType(MappingType);

	FModifyContextOptions ModifyContextOptions;
	ModifyContextOptions.bForceImmediately = true;
	PlayerSubsystem->AddMappingContext(InputMappingContext, 0, ModifyContextOptions);
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromGamepadDeviceID(int DeviceID)
{
	int* Find = PlayerIndexFromGamepadID.Find(DeviceID);
	return Find ? *Find : -1;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToGamepadDeviceID(int DeviceID)
{
	int buff = LastAssignedPlayerIndex;
	PlayerIndexFromGamepadID.Add(DeviceID, LastAssignedPlayerIndex);
	LastAssignedPlayerIndex++;
	return buff;
}

void ULocalMultiplayerSubsystem::AssignGamepadInputMapping(int PlayerIndex,
	ELocalMultiplayerInputMappingType MappingType) const
{
	ULocalPlayer* LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(PlayerIndex);
	UEnhancedInputLocalPlayerSubsystem* PlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (PlayerSubsystem == nullptr) return;
	
	const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();

	TObjectPtr<UInputMappingContext> InputMappingContext = LocalMultiplayerSettings->GamepadProfileData.GetIMCFromType(MappingType);
	
	FModifyContextOptions ModifyContextOptions;
	ModifyContextOptions.bForceImmediately = true;
	PlayerSubsystem->AddMappingContext(InputMappingContext, 0, ModifyContextOptions);
}
