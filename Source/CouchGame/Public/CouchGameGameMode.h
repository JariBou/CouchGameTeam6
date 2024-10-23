// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SfCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "CouchGameGameMode.generated.h"

UCLASS(minimalapi)
class ACouchGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACouchGameGameMode();

	virtual void BeginPlay() override;

private:
	void CreateAndInitPlayers() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASfCharacter> SfCharacterBpClass;
};



