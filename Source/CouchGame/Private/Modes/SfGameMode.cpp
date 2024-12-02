// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/SfGameMode.h"

#include <Systems/CharacterSelectionSubsystem.h>

#include "LocalMultiplayerSettings.h"
#include "LocalMultiplayerSubsystem.h"
#include "Characters/CharacterSettings.h"
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

	UCharacterSelectionSubsystem* CharacterSelectionSubsystem = GetGameInstance()->GetSubsystem<UCharacterSelectionSubsystem>();

	//TODO this should go to the MenuSelectionGamemode
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
		/*
		const FPlayerSelectionInfo& SelectionInfo = CharacterSelectionSubsystem->GetPlayerSelectionInfoFromArray(i);
		SelectionInfo.PlayerTeam;
		SelectionInfo.PlayerController->Possess(NewCharacter);

		// Remove autoposses
		// RemoveSetting player type in here, handled by DesignRandomKnight()
		*/

		ETeam NewPlayerTeam = i%2 > 0 ? Team2 : Team1;
		NewCharacter->PlayerTeam = NewPlayerTeam;
		TeamMap[NewPlayerTeam].AddPlayer(NewCharacter);

		TypeOfPlayer TypeOfPlayer = i/2 > 0 ? Knight : Squire;
		NewCharacter->PlayerType = TypeOfPlayer;

		const UCharacterSettings* CharacterSettings = GetDefault<UCharacterSettings>();
		NewCharacter->ChangeSkeletalMesh(CharacterSettings->CharacterInputDatas[TypeOfPlayer].Mesh.LoadSynchronous());

		NewCharacter->FinishSpawning(SpawnPoint->GetTransform());
		i++;
	}

	TeamMap[Team1].SelectRandomKnight();
	TeamMap[Team2].SelectRandomKnight();


	Respawner = NewObject<URespawner>(this, URespawner::StaticClass());
	Respawner->Initialize(this);
}

void ASfGameMode::NotifyPlayerKilled(ASfCharacter* Killer, ASfCharacter* Dead)
{
	TeamScoreMap[Killer->PlayerTeam]++;

	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "respawn");
	
	if (isGameOver)
	{
		Dead->Destroy();
		return;
	}

	if (Killer != nullptr)
	{
		if (Killer->PlayerType == Knight && Dead->PlayerTeam != Killer->PlayerTeam && Dead->PlayerTeam == Knight) --TeamMap[Dead->PlayerTeam].Lives; // T'es content Jerem?
	}
	
	const FRespawnData respawnData {
		Dead->PlayerTeam,
		Dead->GetController(),
	};
	Dead->GetController()->UnPossess();
	TeamMap[Dead->PlayerTeam].RemovePlayer(Dead);
	Dead->Destroy();

	TeamMap[Dead->PlayerTeam].Players[0]->ChangePlayerType(Knight);
	
	ASfCharacter* NewCharacter = Respawner->StartDeferredRespawn(respawnData);
	
	Respawner->EndDeferredRespawn(respawnData, NewCharacter);

	TeamMap[Dead->PlayerTeam].AddPlayer(NewCharacter);

	if (CheckEndOfGame())
	{
		OnEndOfGame();
	}
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

void ASfGameMode::OnEndOfGame()
{
	//TODO Clément
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

const TMap<TEnumAsByte<ETeam>, FTeamInfo>& ASfGameMode::GetTeamMap() const
{
	return TeamMap;
}
