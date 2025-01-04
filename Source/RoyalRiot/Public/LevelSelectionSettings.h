#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LevelSelectionSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "LevelSelectionSettings"))
class ROYALRIOT_API ULevelSelectionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "LevelSelectionSettings")
	TArray<TSoftObjectPtr<UWorld>> Levels;

	UPROPERTY(Config, VisibleAnywhere, Category = "LevelSelectionSettings")
	FName SelectedLevelName;
	
	TObjectPtr<UWorld> LoadWorldAsset(const TSoftObjectPtr<UWorld>& WorldToLoad) const;
	
};
