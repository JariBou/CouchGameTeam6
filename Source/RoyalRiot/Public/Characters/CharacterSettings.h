// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CharacterSettings.generated.h"

class UMenuInputData;
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
	uint8 MaxHealth = 0xFF;
	
	UPROPERTY(EditAnywhere)
	float InvincibilityTimeAfterHit = .1f;

	UPROPERTY(EditAnywhere)
	float KnockbackForce = .1f;

	UPROPERTY(EditAnywhere)
	FVector Scale = FVector(1, 1, 1);
	
};

UENUM()
enum EDefaultSpawnInfo
{
	NoDefaultSpawnInfo = 0,
	Team1_K = 1,
	Team1_S = 2,
	Team2_K = 3,
	Team2_S = 4,
};

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Character Settings"))
class ROYALRIOT_API UCharacterSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	TObjectPtr<USfCharacterInputData> GetInputDataFromPlayerType(TEnumAsByte<TypeOfPlayer> Type) const;

	TObjectPtr<UMenuInputData> GetMenuInputData() const;

	UPROPERTY(Config, EditAnywhere, Category="Settings")
	TMap<TEnumAsByte<TypeOfPlayer>, FCharacterSettingsData> CharacterInputDatas;

	UPROPERTY(Config, EditAnywhere, Category="Settings")
	TSoftObjectPtr<UMenuInputData> MenuInputData;
	
	UPROPERTY(Config, EditAnywhere, Category="Settings")
	float RespawnInvincibilityTime;

	UPROPERTY(Config, EditAnywhere, Category="Settings")
	float InvincibilityTimeAfterGive = 1;

	UPROPERTY(Config, EditAnywhere, Category="Settings")
	float RespawnTime = 2;

	UPROPERTY(Config, VisibleAnywhere, Category="Settings")
	TMap<uint8, TEnumAsByte<EDefaultSpawnInfo>> DefaultSpawnInfo;

	UPROPERTY(Config, VisibleAnywhere, Category="Settings")
	bool UseDefaultSpawnInfo;
};
