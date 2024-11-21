// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SplinePoolComponent.h"
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

	UPROPERTY(EditAnywhere, meta=(GetOptions="GetWeaponNames")) // <==== ici je lui dit que les options sont données par la méthode GetWeaponNames
	TArray<FName> Weapons;

	UPROPERTY(EditAnywhere)
	float PercentChance = 0;
	//recompile stp
};

USTRUCT()
struct FWeaponsRarityList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EWeaponRarity>, FWeaponsList> WeaponListRarity;

	UPROPERTY(EditAnywhere)
	uint8 NumberOfSpawnedWeapons = 1;
	
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Time between spawn of number of spawned weapons"))
	uint8 TimeBetweenSpawns = 1;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Time between spawn cycles"))
	uint8 TimeBetweenForging = 15;

	UPROPERTY(EditAnywhere)
	uint8 UpgradesAfterXForgings = 3;
	
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

	void InitializeSpawnPoints();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void ForgeLoop();

private:
	UPROPERTY()
	uint8 ForgingsDoneThisLevel = 0;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SpawnWeapon(FName WeaponName);

	UPROPERTY(EditAnywhere)
	TArray<AActor*> SpawnPointActors;
	
private:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EForgeLevel> ForgeLevel = Level1;

	UPROPERTY(EditAnywhere)
	uint8 InitialSpawnDelay = 1;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> WeaponsDataTable;

	// UPROPERTY(EditAnywhere)
	// TArray<FTransform> SpawnPoints;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EForgeLevel>, FWeaponsRarityList> ForgeMap;

	UPROPERTY(VisibleAnywhere)
	USplinePoolComponent* SplinePoolComponent;

	UFUNCTION()
	TArray<FName> GetWeaponNames() const;

	UPROPERTY()
	TArray<FVector> SpawnPoints;
};
