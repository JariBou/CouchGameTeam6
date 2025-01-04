// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Utils/ArrayUtils.h>

#include "CoreMinimal.h"
#include "Teams.h"
#include "Characters/SfCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "Systems/Respawner.h"
#include "SfGameMode.generated.h"

class UUserWidgetBlueprint;

USTRUCT(BlueprintType)
struct FTeamInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<ETeam> Team = Team1;

	UPROPERTY(BlueprintReadOnly)
	TArray<ASfCharacter*> Players;

	UPROPERTY(BlueprintReadOnly)
	uint8 Lives = 1; // <==== UINT8

	void AddPlayer(ASfCharacter* Player)
	{
		Players.Add(Player);
	}

	void SelectRandomKnight() const
	{
		ASfCharacter* RandomPlayer;
		UArrayUtils::GetRandomElement(Players, RandomPlayer);
		RandomPlayer->ChangePlayerType(Knight);
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
class ROYALRIOT_API ASfGameMode : public AGameModeBase
{
public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamScoreChange, ETeam, TeamChanged, int, NewLives);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerRespawn, ETeam, TeamChanged, float, TimerValue, EIndicatorDisplayAction, DisplayAction);

	UPROPERTY(BlueprintAssignable, Category="Event")
	FOnTeamScoreChange OnTeamScoreChange;

	UPROPERTY(BlueprintAssignable, Category="Event")
	FOnPlayerRespawn OnPlayerRespawn;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> EndGameMenu;

	ASfGameMode();
	
	UFUNCTION(BlueprintCallable)
	void NotifyPlayerKilled(ASfCharacter* Killer, ASfCharacter* Dead);

	UFUNCTION(BlueprintCallable)
	float GetTimer();

	UFUNCTION(BlueprintCallable)
	void SetTimeDilation(float NewTimeDilation);
	
private:
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	URespawner* Respawner;

	UPROPERTY()
	float RespawnTime;

	virtual void StartPlay() override;

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	bool CheckEndOfGame();
	
	UFUNCTION(BlueprintCallable)
	void OnEndOfGame();
	
private:
	void CreateAndInitPlayers() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASfCharacter> SfCharacterBpClass;

	UPROPERTY(EditAnywhere)
	float MaxTime;

	UPROPERTY()
	float Timer;

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

	UPROPERTY()
	UIndicatorWidget* RespawnIndicator;

public:
	UFUNCTION(BlueprintCallable)
	const TMap<TEnumAsByte<ETeam>, FTeamInfo>& GetTeamMap() const;

	
	
};
