// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/Systems/Respawner.h"

#include <Utils/TransformUtils.h>

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
	Character->SetInvincibility(true);

	return Character;
}

ASfCharacter* URespawner::QueueRespawn(FRespawnData RespawnData, float Delay)
{
	if (TeamRespawnDelegateMap.Contains(RespawnData.Team))
	{
		FQueuedRespawnData QueuedRespawnData = TeamRespawnDelegateMap[RespawnData.Team];
		GameMode->GetWorldTimerManager().ClearTimer(QueuedRespawnData.TimerHandle);
		EndDeferredRespawn(QueuedRespawnData.RespawnData, QueuedRespawnData.Character);
	}
	
	ASfCharacter* Character = GetWorld()->SpawnActorDeferred<ASfCharacter>(GameMode->GetSfCharacterBpClass(), RespawnPoint + UTransformUtils::MakeTransformFromLocation(FVector(0, 0, 10000)), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	Character->PlayerTeam = RespawnData.Team;
	Character->PlayerType = RespawnData.TypeOfPlayer;
	Character->SetInvincibility(true);

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;

	FQueuedRespawnData QueuedRespawnData{
		TimerHandle,
		RespawnData,
		Character
	};
	
	TeamRespawnDelegateMap.Add(RespawnData.Team, QueuedRespawnData);
	
	TimerDelegate.BindUObject(this, &URespawner::EndDeferredRespawn, RespawnData, Character);
	GameMode->GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, Delay, false);


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
	
	Character->TriggerRespawnSound.Broadcast();

	if (TeamRespawnDelegateMap.Contains(RespawnData.Team)) TeamRespawnDelegateMap.Remove(RespawnData.Team);

	// RespawnMap.Remove(RespawnData);
}
