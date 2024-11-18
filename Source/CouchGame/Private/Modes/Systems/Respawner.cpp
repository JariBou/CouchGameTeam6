// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/Systems/Respawner.h"

#include "Characters/SfCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/SfGameMode.h"

void URespawner::Initialize(ASfGameMode* inGameMode)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "RespawnPoint", Actors);
	if (Actors.Num() > 0) RespawnPoint = Actors[0]->GetTransform();

	GameMode = inGameMode;
}

ASfCharacter* URespawner::StartDeferredRespawn(FRespawnData RespawnData)
{
	ASfCharacter* Character = Cast<ASfCharacter>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), GameMode->GetSfCharacterBpClass(), RespawnPoint, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

	// RespawnMap.Add(RespawnData, Character);
	Character->PlayerTeam = RespawnData.Team;
	Character->PlayerType = Squire;

	return Character;
}

void URespawner::EndDeferredRespawn(FRespawnData RespawnData, ASfCharacter* Character)
{
	RespawnData.PlayerController->Possess(Character);
	
	Character->FinishSpawning(RespawnPoint);
	// RespawnMap.Remove(RespawnData);
}
