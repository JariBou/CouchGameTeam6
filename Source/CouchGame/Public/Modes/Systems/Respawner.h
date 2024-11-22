// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
	TEnumAsByte<ETeam> Team;

	UPROPERTY()
	AController* PlayerController;

	UPROPERTY()
	TEnumAsByte<TypeOfPlayer> TypeOfPlayer = Squire;
};

/**
 * 
 */
UCLASS()
class COUCHGAME_API URespawner : public UObject
{
	GENERATED_BODY()

public:

	void Initialize(ASfGameMode* inGameMode);
	
	UFUNCTION(BlueprintCallable, Category = "Respawner")
	ASfCharacter* StartDeferredRespawn(FRespawnData RespawnData);

	UFUNCTION(BlueprintCallable, Category = "Respawner")
	void EndDeferredRespawn(FRespawnData RespawnData, ASfCharacter* Character);

	UPROPERTY()
	FTransform RespawnPoint;

	UPROPERTY()
	ASfGameMode* GameMode;
};
