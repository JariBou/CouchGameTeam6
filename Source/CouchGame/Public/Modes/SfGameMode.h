// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Teams.h"
#include "Characters/SfCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "SfGameMode.generated.h"

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

	virtual void BeginPlay() override;

	

	UFUNCTION(BlueprintCallable)
	bool CheckEndOfGame();

private:
	void CreateAndInitPlayers() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASfCharacter> SfCharacterBpClass;

	UPROPERTY(EditAnywhere)
	uint8 DeathCountTarget;

	UPROPERTY()
	TMap<TEnumAsByte<ETeam>, uint8> TeamScoreMap = {
		 
	};
};
