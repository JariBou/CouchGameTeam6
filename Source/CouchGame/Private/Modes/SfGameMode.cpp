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

	for (ETeam Team : {Team1, Team2})
	{
		FTeamInfo NewInfo {
		.Team = Team,
		.Players = {},
		.Lives = TeamLives
		};
		TeamMap.Add(Team, NewInfo);
	}
	
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
		
		ETeam NewPlayerTeam = i%2 > 0 ? Team2 : Team1;
		NewCharacter->PlayerTeam = NewPlayerTeam;
		TeamMap[NewPlayerTeam].AddPlayer(NewCharacter);
		
		NewCharacter->PlayerType = i/2 > 0 ? Knight : Squire;
		NewCharacter->FinishSpawning(SpawnPoint->GetTransform());
		i++;
	}


	Respawner = NewObject<URespawner>(this, URespawner::StaticClass());
	Respawner->Initialize(this);
}

void ASfGameMode::NotifyPlayerKilled(ASfCharacter* Killer, ASfCharacter* Dead)
{
	TeamScoreMap[Killer->PlayerTeam]++;

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "Oh Fils de pute");
	
	if (isGameOver)
	{
		//TODO Clément
		// Oooh
		Dead->Destroy();
		return;
	}
	
	--TeamMap[Dead->PlayerTeam].Lives; // T'es content Jerem?
	

	const FRespawnData respawnData {
		Dead->PlayerTeam,
		Dead->GetController(),
	};
	Dead->GetController()->UnPossess();
	TeamMap[Dead->PlayerTeam].RemovePlayer(Dead);
	Dead->Destroy();

	TeamMap[Dead->PlayerTeam].Players[0]->ChangePlayerType(Knight);
	
	ASfCharacter* NewCharacter = Respawner->StartDeferredRespawn(respawnData);

	TeamMap[Dead->PlayerTeam].AddPlayer(NewCharacter);
	
	Respawner->EndDeferredRespawn(respawnData, NewCharacter);

	CheckEndOfGame();
}

bool ASfGameMode::CheckEndOfGame()
{
	isGameOver = false;
	for (const auto& [_, TeamInfo] : TeamMap)
	{
		// Proceed to kill myself after that
		isGameOver |= TeamInfo.Lives <= 0 ? 1 : 0;
	}
	return isGameOver;
	return TeamScoreMap[Team1] >= TeamLives || TeamScoreMap[Team2] >= TeamLives;
}

void ASfGameMode::CreateAndInitPlayers() const
{
	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (GameInstance == nullptr) return;

	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GameInstance->GetSubsystem<ULocalMultiplayerSubsystem>();
	if (LocalMultiplayerSubsystem == nullptr) return;

	LocalMultiplayerSubsystem->CreateAndInitPlayers(ELocalMultiplayerInputMappingType::InGame);
}

const TSubclassOf<ASfCharacter>& ASfGameMode::GetSfCharacterBpClass() const
{
	return SfCharacterBpClass;
}
