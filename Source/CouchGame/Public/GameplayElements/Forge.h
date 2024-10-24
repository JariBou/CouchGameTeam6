// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponsDataAsset.h"
#include "GameFramework/Actor.h"
#include "Forge.generated.h"

UENUM(BlueprintType)
enum EForgeLevel : uint8
{
	Level1 = 0 ,
	Level2 = 1 ,
	Level3 = 2 ,
	Level4 = 3 ,
	Level5 = 4 ,
};

USTRUCT()
struct FWeaponsList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FWeaponInfo> Weapons;

	UPROPERTY(EditAnywhere)
	uint8 PercentChance;
};

USTRUCT()
struct FWeaponsRarityList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EWeaponRarity>, FWeaponsList> WeaponListRarity;
};

UCLASS()
class COUCHGAME_API AForge : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AForge();

	/**
	 * 
	 * @param NumberToSpawn Number of weapons to spawn in this attempt
	 * @param SpawnDelayOfAttempts Spawn delay between spawns in the same attempt
	 */
	UFUNCTION(BlueprintCallable, Category = "Forge")
	void SpawnWeaponsAtRandomLocation(int NumberToSpawn, int SpawnDelayOfAttempts);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TArray<FTransform> SpawnPoints;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EForgeLevel>, FWeaponsRarityList> ForgeMap;
};
