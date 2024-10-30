// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/SfGameMode.h"

#include "LocalMultiplayerSettings.h"
#include "LocalMultiplayerSubsystem.h"
#include "Characters/SfCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ASfGameMode::ASfGameMode()
{
	
}

void ASfGameMode::BeginPlay()
{
	Super::BeginPlay();
	TeamScoreMap.Add(Team1);
	TeamScoreMap.Add(Team2);
	
	CreateAndInitPlayers();

	TArray<AActor*> outPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), outPlayerStarts);
	uint8 i = 0;
	for (AActor* SpawnPoint : outPlayerStarts)
	{
		if (SfCharacterBpClass == nullptr) continue;

		ASfCharacter* NewCharacter = GetWorld()->SpawnActorDeferred<ASfCharacter>(SfCharacterBpClass,SpawnPoint->GetTransform());
		if (NewCharacter == nullptr) continue;

		NewCharacter->AutoPossessPlayer = SpawnPoint->AutoReceiveInput;
		NewCharacter->PlayerTeam = i%2 > 0 ? Team2 : Team1;
		NewCharacter->FinishSpawning(SpawnPoint->GetTransform());
		i++;
	}
}

void ASfGameMode::OnPlayerKilled(ASfCharacter* Killer, ASfCharacter* Dead)
{
	TeamScoreMap[Killer->PlayerTeam]++;
}

bool ASfGameMode::CheckEndOfGame()
{
	return TeamScoreMap[Team1] >= DeathCountTarget || TeamScoreMap[Team2] >= DeathCountTarget;
}

void ASfGameMode::CreateAndInitPlayers() const
{
	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (GameInstance == nullptr) return;

	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GameInstance->GetSubsystem<ULocalMultiplayerSubsystem>();
	if (LocalMultiplayerSubsystem == nullptr) return;

	LocalMultiplayerSubsystem->CreateAndInitPlayers(ELocalMultiplayerInputMappingType::InGame);
}
