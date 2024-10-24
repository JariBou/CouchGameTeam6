// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayElements/Forge.h"

#include "Weapon.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AForge::AForge()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AForge::SpawnWeaponsAtRandomLocation(int NumberToSpawn, int SpawnDelayOfAttempts)
{
	SpawnRandomWeapon();
	float SpawnDelay = SpawnDelayOfAttempts;
	for (int i = 0; i < NumberToSpawn-1; i++)
	{
		FTimerHandle SpawnDelayTimerHandle;
		GetGameInstance()->GetTimerManager().SetTimer(SpawnDelayTimerHandle, this, &AForge::SpawnRandomWeapon, SpawnDelay);
		SpawnDelay += SpawnDelayOfAttempts;
	}
}

void AForge::SpawnRandomWeapon()
{
	FWeaponsRarityList WeaponsRarityList = ForgeMap[ForgeLevel];
	
	WeaponsRarityList.WeaponListRarity.ValueSort([](const FWeaponsList &A, const FWeaponsList &B) { return A.PercentChance < B.PercentChance; });

	TArray<TEnumAsByte<EWeaponRarity>> WeaponRarities;
	WeaponsRarityList.WeaponListRarity.GetKeys(WeaponRarities);
	float Rand = FMath::RandRange(0.f, 1.f);
	int8 top = 1;

	for (TEnumAsByte<EWeaponRarity> WeaponRarity : WeaponRarities)
	{
		if (top - WeaponsRarityList[WeaponRarity].PercentChance <= Rand)
		{
			float RandWeaponIndex = FMath::RandRange(0, WeaponsRarityList[WeaponRarity].Weapons.Num()-1);
			FName WeaponRowName = WeaponsRarityList[WeaponRarity].Weapons[RandWeaponIndex];

			FWeaponInfo* WeaponInfo = WeaponsDataTable->FindRow<FWeaponInfo>(WeaponRowName, "");

			float RandomSpawnpointIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
			FTransform SpawnPointTransform = SpawnPoints[RandomSpawnpointIndex];

			AWeapon* NewWeapon = Cast<AWeapon>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetGameInstance()->GetWorld(), AWeapon::StaticClass(), SpawnPointTransform));
			NewWeapon->CurrentDataRow = *WeaponInfo;
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("New Weapon Created"));
			NewWeapon->FinishSpawning(SpawnPointTransform);
		}
	}
}

// Called when the game starts or when spawned
void AForge::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeaponsAtRandomLocation(2, 5);
}

// Called every frame
void AForge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<FName> AForge::GetWeaponNames() const
{
	if (WeaponsDataTable == nullptr) return TArray<FName>({"NO DATA TABLE"});
	return WeaponsDataTable->GetRowNames();
}

