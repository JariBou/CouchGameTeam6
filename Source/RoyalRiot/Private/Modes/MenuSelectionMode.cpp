// Fill out your copyright notice in the Description page of Project Settings.


#include <Modes/MenuSelectionMode.h>
#include <LocalMultiplayerSettings.h>
#include <LocalMultiplayerSubsystem.h>

void AMenuSelectionMode::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterSelectionSubsystem = GetGameInstance()->GetSubsystem<UCharacterSelectionSubsystem>();

	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();
	if (LocalMultiplayerSubsystem == nullptr) return;

	LocalMultiplayerSubsystem->CreateAndInitPlayers(ELocalMultiplayerInputMappingType::Menu);

	// TODO
}
