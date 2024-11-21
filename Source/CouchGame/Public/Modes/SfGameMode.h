// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Teams.h"
#include "Characters/SfCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "Systems/Respawner.h"
#include "SfGameMode.generated.h"

USTRUCT(BlueprintType)
struct FTeamInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<ETeam> Team;

	UPROPERTY(BlueprintReadOnly)
	TArray<ASfCharacter*> Players;

	UPROPERTY(BlueprintReadOnly)
	uint8 Lives; // <==== UINT8

	void AddPlayer(ASfCharacter* Player)
	{
		Players.Add(Player);
	}

	void RemovePlayer(ASfCharacter* Player)
	{
		Players.Remove(Player);
	}

	ASfCharacter* GetKnight()
	{
		return *Players.FindByPredicate([](const ASfCharacter* Player)
		{
			return Player->PlayerType == Knight;
		});
	}

	// ~FTeamInfo() = default;
	// // je crois que je t'emmerde Jerem
};

class ASfCharacter;
/**
 * 
 */
UCLASS()
class COUCHGAME_API ASfGameMode : public AGameModeBase
{
public:
	ASfGameMode();
	UFUNCTION(BlueprintCallable)
	void NotifyPlayerKilled(ASfCharacter* Killer, ASfCharacter* Dead);
	
private:
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	URespawner* Respawner;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool CheckEndOfGame();
	
	UFUNCTION(BlueprintCallable)
	void OnEndOfGame();
	
private:
	void CreateAndInitPlayers() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASfCharacter> SfCharacterBpClass;

public:
	const TSubclassOf<ASfCharacter>& GetSfCharacterBpClass() const;

private:
	UPROPERTY()
	bool isGameOver = false;

	UPROPERTY(EditAnywhere)
	uint8 TeamLives;

	UPROPERTY()
	TMap<TEnumAsByte<ETeam>, uint8> TeamScoreMap = {
	};

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<ETeam>, FTeamInfo> TeamMap = {
	};

public:
	UFUNCTION(BlueprintCallable)
	const TMap<TEnumAsByte<ETeam>, FTeamInfo>& GetTeamMap() const;
	
};
