// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Teams.h>

#include "CoreMinimal.h"
#include "TypeOfPickable.h"
#include "UObject/Object.h"
#include "Respawner.generated.h"

class ASfGameMode;
class ASfCharacter;
enum ETeam : uint8;

USTRUCT(BlueprintType)
struct FRespawnData
{
	GENERATED_BODY()

	public:
	UPROPERTY()
	TEnumAsByte<ETeam> Team = Team1;

	UPROPERTY()
	AController* PlayerController;

	UPROPERTY()
	TEnumAsByte<TypeOfPlayer> TypeOfPlayer = Squire;
	
	UPROPERTY()
	bool HadToFastRespawn = false;
};

USTRUCT(BlueprintType)
struct FQueuedRespawnData
{
	GENERATED_BODY()

	public:
	UPROPERTY()
	FTimerHandle TimerHandle;
	
	UPROPERTY()
	FRespawnData RespawnData;

	UPROPERTY()
	ASfCharacter* Character;
};

/**
 * 
 */
UCLASS()
class ROYALRIOT_API URespawner : public UObject
{
	GENERATED_BODY()

public:

	void Initialize(ASfGameMode* inGameMode);
	
	UFUNCTION(BlueprintCallable, Category = "Respawner")
	ASfCharacter* StartDeferredRespawn(FRespawnData RespawnData);

	UFUNCTION(BlueprintCallable, Category = "Respawner")
	ASfCharacter* QueueRespawn(FRespawnData RespawnData, float Delay);

	UFUNCTION(BlueprintCallable, Category = "Respawner")
	void EndDeferredRespawn(FRespawnData RespawnData, ASfCharacter* Character);

	UPROPERTY()
	FTransform RespawnPoint;

	UPROPERTY()
	ASfGameMode* GameMode;

	UPROPERTY()
	TMap<TEnumAsByte<ETeam>, FQueuedRespawnData> TeamRespawnDelegateMap;
};
