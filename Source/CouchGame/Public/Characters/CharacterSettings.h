// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterSettings.generated.h"

class USfCharacterInputData;
enum TypeOfPlayer : uint8;
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Character Settings"))
class COUCHGAME_API UCharacterSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	public:

	USfCharacterInputData* GetInputDataFromPlayerType(TEnumAsByte<TypeOfPlayer> Type) const;
	
	UPROPERTY(Config, EditAnywhere, Category = "Settings")
	TMap<TEnumAsByte<TypeOfPlayer>, TSoftObjectPtr<USfCharacterInputData>> CharacterInputDatas;
};
