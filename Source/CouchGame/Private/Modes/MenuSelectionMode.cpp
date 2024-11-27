// Fill out your copyright notice in the Description page of Project Settings.


#include <Modes/MenuSelectionMode.h>

void AMenuSelectionMode::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterSelectionSubsystem = GetGameInstance()->GetSubsystem<UCharacterSelectionSubsystem>();
}
