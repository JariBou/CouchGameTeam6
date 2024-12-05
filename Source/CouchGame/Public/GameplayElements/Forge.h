// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnPointToolUser.h"
#include "SplinePoolComponent.h"
#include "WeaponsDataAsset.h"
#include "GameFramework/Actor.h"
#include "Forge.generated.h"

UENUM(BlueprintType)
enum EForgeLevel : uint8
{
	RequiredThingButCantBeNone = 0 UMETA(Hidden),
	Level1 = 1 << 0 ,
	Level2 = 1 << 1 ,
	Level3 = 1 << 2 ,
	Level4 = 1 << 3 ,
	Level5 = 1 << 4 ,
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
class COUCHGAME_API AForge : public AActor, public ISpawnPointToolUser
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTriggerSound);

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

	virtual TArray<AActor*> GetAllSpawnPoints_Implementation() override;

	virtual void SetSpawnPoints_Implementation(const TArray<AActor*>& NewRespawnPoints) override;

	virtual void AddSpawnPoint_Implementation(AActor* NewSpawnPoint) override;

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

	UPROPERTY(BlueprintAssignable, Category="event")
	FTriggerSound TriggerWeaponSpawnSound;
	
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
