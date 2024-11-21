// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CharacterSettings.generated.h"

class USfCharacterInputData;
enum TypeOfPlayer : uint8;

USTRUCT(BlueprintType)
struct FCharacterSettingsData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USfCharacterInputData> InputData;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAnimInstance> AnimBlueprint;

	UPROPERTY(EditAnywhere)
	uint8 MaxHealth;
};

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Character Settings"))
class COUCHGAME_API UCharacterSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	TObjectPtr<USfCharacterInputData> GetInputDataFromPlayerType(TEnumAsByte<TypeOfPlayer> Type) const;

	UPROPERTY(Config, EditAnywhere, Category="Settings")
	TMap<TEnumAsByte<TypeOfPlayer>, FCharacterSettingsData> CharacterInputDatas;
};
