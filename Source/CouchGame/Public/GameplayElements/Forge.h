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

	UPROPERTY(EditAnywhere, meta=(GetOptions="GetWeaponNames"))
	TArray<FName> Weapons;

	UPROPERTY(EditAnywhere)
	uint8 PercentChance;
};

USTRUCT()
struct FWeaponsRarityList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EWeaponRarity>, FWeaponsList> WeaponListRarity;
	
	FORCEINLINE       FWeaponsList& operator[](TEnumAsByte<EWeaponRarity> Key) { return WeaponListRarity[Key]; }
	FORCEINLINE const FWeaponsList& operator[](TEnumAsByte<EWeaponRarity> Key) const { return WeaponListRarity[Key]; }
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
	void SpawnRandomWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EForgeLevel> ForgeLevel = Level1;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> WeaponsDataTable;

	UPROPERTY(EditAnywhere)
	TArray<FTransform> SpawnPoints;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EForgeLevel>, FWeaponsRarityList> ForgeMap;

	UFUNCTION()
	TArray<FName> GetWeaponNames() const;
};
