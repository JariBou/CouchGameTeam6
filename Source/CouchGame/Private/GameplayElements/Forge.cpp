// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayElements/Forge.h"

#include "SplinePoolComponent.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AForge::AForge()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplinePoolComponent = CreateDefaultSubobject<USplinePoolComponent>(TEXT("Spline Pool Component"));
}

void AForge::SpawnWeaponsAtRandomLocation(int NumberToSpawn, int SpawnDelayOfAttempts)
{
	ForgingsDoneThisLevel++;
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

			SpawnWeapon(WeaponRowName);

			//SplinePoolComponent->StartSplineForWeapon(NewWeapon, GetTransform().GetLocation(), SpawnPointLocation);
		}
	}
}

// Called when the game starts or when spawned
void AForge::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle SpawnDelayTimerHandle;
	GetGameInstance()->GetTimerManager().SetTimer(SpawnDelayTimerHandle, this, &AForge::ForgeLoop, ForgeMap[ForgeLevel].TimeBetweenForging);;
}

void AForge::ForgeLoop()
{
	while (!ForgeMap.Contains(ForgeLevel))
	{
		ForgeLevel = static_cast<EForgeLevel>(ForgeLevel - 1);
	}
	
	FWeaponsRarityList WeaponsRarityList = ForgeMap[ForgeLevel];
	SpawnWeaponsAtRandomLocation(WeaponsRarityList.NumberOfSpawnedWeapons, WeaponsRarityList.TimeBetweenSpawns);

	if (ForgingsDoneThisLevel >= WeaponsRarityList.UpgradesAfterXForgings)
	{
		ForgeLevel = static_cast<EForgeLevel>(ForgeLevel + 1);
		ForgingsDoneThisLevel = 0;
	}
	
	FTimerHandle SpawnDelayTimerHandle;
	GetGameInstance()->GetTimerManager().SetTimer(SpawnDelayTimerHandle, this, &AForge::ForgeLoop, WeaponsRarityList.TimeBetweenForging);
}

// Called every frame
void AForge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AForge::SpawnWeapon(FName WeaponName)
{
	FWeaponInfo* WeaponInfo = WeaponsDataTable->FindRow<FWeaponInfo>(WeaponName, "");

	float RandomSpawnpointIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
	FVector SpawnPointLocation = SpawnPoints[RandomSpawnpointIndex].GetLocation();
	FTransform WeaponSpawnTransform = GetActorTransform();
	// FTransform SpawnPointTransform = FTransform();

	AWeapon* NewWeapon = Cast<AWeapon>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetGameInstance()->GetWorld(), WeaponInfo->WeaponBP, WeaponSpawnTransform));
	NewWeapon->SetCurrentData(*WeaponInfo);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("New Weapon Created"));
	NewWeapon->FinishSpawning(WeaponSpawnTransform);

	// FVector TrajectVector = SpawnPointLocation - GetActorTransform().GetLocation();
	// double Distance = TrajectVector.Length();
	// FVector Direction = FVector(TrajectVector.X, TrajectVector.Y, 0);
	// Direction.Normalize();
	//
	// FVector Normal = Direction * FVector(0, 0, 1);
	// (Direction, FVector::UpVector);
	//
	//
	// FVector WeaponImpulse = FVector(0, 0, 0);

	FVector TossVelocity;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), TossVelocity, GetTransform().GetLocation(), SpawnPointLocation, 0, 0.3f);

	FPredictProjectilePathParams PredictParams;
	PredictParams.LaunchVelocity = TossVelocity;
	PredictParams.StartLocation = GetTransform().GetLocation();
	PredictParams.OverrideGravityZ = 0;
	PredictParams.ActorsToIgnore = {GetOwner()};
	//PredictParams.ObjectTypes = {EObjectTypeQuery::ObjectTypeQuery1};
	PredictParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	PredictParams.DrawDebugTime = 3;
	PredictParams.MaxSimTime = 3;


	FPredictProjectilePathResult PredictResult;
	UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TossVelocity.ToString());

	Cast<UPrimitiveComponent>(NewWeapon->GetRootComponent())->AddImpulse(TossVelocity, NAME_None, true);
}

TArray<FName> AForge::GetWeaponNames() const
{
	if (WeaponsDataTable == nullptr) return TArray<FName>({"NO DATA TABLE"});
	return WeaponsDataTable->GetRowNames();
}

