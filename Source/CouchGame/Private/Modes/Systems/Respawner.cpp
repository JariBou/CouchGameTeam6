// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/Systems/Respawner.h"

#include "Characters/CharacterSettings.h"
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
	ASfCharacter* Character = GetWorld()->SpawnActorDeferred<ASfCharacter>(GameMode->GetSfCharacterBpClass(), RespawnPoint, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	// ASfCharacter* Character = Cast<ASfCharacter>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), GameMode->GetSfCharacterBpClass(), RespawnPoint, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

	// RespawnMap.Add(RespawnData, Character);
	Character->PlayerTeam = RespawnData.Team;
	Character->PlayerType = RespawnData.TypeOfPlayer;
	// Character->SetInvincibility(true);

	return Character;
}

void URespawner::EndDeferredRespawn(FRespawnData RespawnData, ASfCharacter* Character)
{
	RespawnData.PlayerController->Possess(Character);

	const UCharacterSettings* CharacterSettings = GetDefault<UCharacterSettings>();
	USkeletalMesh* SkeletalMesh = CharacterSettings->CharacterInputDatas[RespawnData.TypeOfPlayer].Mesh.LoadSynchronous();
	Character->ChangeSkeletalMesh(SkeletalMesh);
	
	FTimerHandle NullHandle;
	Character->GetGameInstance()->GetTimerManager().SetTimer(NullHandle, Character, &ASfCharacter::RemoveInvincibility, CharacterSettings->RespawnInvincibilityTime);
	
	//
	// Character->SetupHealth(CharacterSettings->CharacterInputDatas[RespawnData.TypeOfPlayer].MaxHealth);

	Character->FinishSpawning(RespawnPoint);
	// RespawnMap.Remove(RespawnData);
}
