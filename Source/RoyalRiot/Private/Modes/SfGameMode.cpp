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

	const UCharacterSettings* CharacterSettings = GetDefault<UCharacterSettings>();
	RespawnTime = CharacterSettings->RespawnTime;

	for (ETeam Team : {Team1, Team2})
	{
		FTeamInfo NewInfo {
		.Team = Team,
		.Players = {},
		.Lives = TeamLives
		};
		TeamMap.Add(Team, NewInfo);
		OnTeamScoreChange.Broadcast(Team, TeamLives);
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
		//Assign teams after selection
		const FPlayerSelectionInfo& SelectionInfo = CharacterSelectionSubsystem->GetPlayerSelectionInfoFromArray(i);
		SelectionInfo.PlayerTeam;

		// Possess after finishing spawn IMO
		SelectionInfo.PlayerController->Possess(NewCharacter);

		// Remove autoposses
		// RemoveSetting player type in here, handled by DesignRandomKnight()
		*/

		if (CharacterSettings->UseDefaultSpawnInfo)
		{
			if (!CharacterSettings->DefaultSpawnInfo.Contains(i)) continue;
			switch (CharacterSettings->DefaultSpawnInfo[i])
			{
				case Team1_K:
					NewCharacter->PlayerTeam = Team1;
					NewCharacter->PlayerType = Knight;
					break;
				case Team1_S:
					NewCharacter->PlayerTeam = Team1;
					NewCharacter->PlayerType = Squire;
					break;
				case Team2_K:
					NewCharacter->PlayerTeam = Team2;
					NewCharacter->PlayerType = Knight;
					break;
				case Team2_S:
					NewCharacter->PlayerTeam = Team2;
					NewCharacter->PlayerType = Squire;
					break;
				default:
				case NoDefaultSpawnInfo:
					break;
			}
		} else
		{
			ETeam NewPlayerTeam = i%2 > 0 ? Team2 : Team1;
			NewCharacter->PlayerTeam = NewPlayerTeam;

			TypeOfPlayer TypeOfPlayer = /*i/2 > 0 ? Knight :*/ Squire;
			NewCharacter->PlayerType = TypeOfPlayer;
		}		

		TeamMap[NewCharacter->PlayerTeam].AddPlayer(NewCharacter);
		NewCharacter->ChangeSkeletalMesh(CharacterSettings->CharacterInputDatas[NewCharacter->PlayerType].Mesh.LoadSynchronous());

		NewCharacter->FinishSpawning(SpawnPoint->GetTransform());
		i++;
	}

	if (!CharacterSettings->UseDefaultSpawnInfo)
	{
		TeamMap[Team1].SelectRandomKnight();
		TeamMap[Team2].SelectRandomKnight();
	}


	Respawner = NewObject<URespawner>(this, URespawner::StaticClass());
	Respawner->Initialize(this);

	Timer = MaxTime;
}

void ASfGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (isGameOver) return;
	
	Timer -= DeltaSeconds;

	if (Timer <= 0){
		isGameOver = true;
		OnEndOfGame();
	}
}

void ASfGameMode::NotifyPlayerKilled(ASfCharacter* Killer, ASfCharacter* Dead)
{
	//TeamScoreMap[Killer->PlayerTeam]++;

	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "respawn");
	
	if (isGameOver)
	{
		Dead->Destroy();
		return;
	}

	if (Killer != nullptr)
	{
		if ((Killer->PlayerType == Knight) && (Dead->PlayerTeam != Killer->PlayerTeam) && (Dead->PlayerType == Knight))
		{
			// T'es content Jerem?
			--TeamMap[Dead->PlayerTeam].Lives;
			OnTeamScoreChange.Broadcast(Dead->PlayerTeam, TeamMap[Dead->PlayerTeam].Lives);
		} 
	}
	
	const FRespawnData respawnData {
		Dead->PlayerTeam,
		Dead->GetController(),
	};
	
	respawnData.PlayerController->UnPossess();
	TeamMap[Dead->PlayerTeam].RemovePlayer(Dead);
	Dead->Destroy();
	
	TeamMap[Dead->PlayerTeam].Players[0]->ChangePlayerType(Knight, true);
	
	ASfCharacter* NewCharacter = Respawner->QueueRespawn(respawnData, RespawnTime);
	
	//Respawner->EndDeferredRespawn(respawnData, NewCharacter);

	TeamMap[Dead->PlayerTeam].AddPlayer(NewCharacter);

	if (CheckEndOfGame())
	{
		OnEndOfGame();
	}
}

float ASfGameMode::GetTimer()
{
	return Timer;
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
	// TODO: Clément
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
