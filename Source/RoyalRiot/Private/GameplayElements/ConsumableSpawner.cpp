// Fill out your copyright notice in the Description page of Project Settings.


#include <GameplayElements/ConsumableSpawner.h>
#include <Utils/ArrayUtils.h>


// Sets default values
AConsumableSpawner::AConsumableSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AConsumableSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (InitialSpawnDelay > 0)
	{
		FTimerHandle NullHandle;
		GetWorldTimerManager().SetTimer(NullHandle, this, &AConsumableSpawner::SpawnRandomConsumable, InitialSpawnDelay);
	} else
	{
		SpawnRandomConsumable();
	}
}

void AConsumableSpawner::SpawnRandomConsumable()
{
	TSubclassOf<AConsumable> ConsumableClass = *UArrayUtils::GetRandomElement(PossibleConsumables);
	Consumable = GetWorld()->SpawnActor<AConsumable>(ConsumableClass, GetActorLocation(), GetActorRotation());
	Consumable->OnPickedUp.AddDynamic(this, &AConsumableSpawner::OnSpawnedItemPickedUp);
}

void AConsumableSpawner::OnSpawnedItemPickedUp()
{
	Consumable->OnPickedUp.RemoveDynamic(this, &AConsumableSpawner::OnSpawnedItemPickedUp);
	Consumable = nullptr;
	FTimerHandle NullHandle;
	GetWorldTimerManager().SetTimer(NullHandle, this, &AConsumableSpawner::SpawnRandomConsumable, SpawnDelay);
}

// Called every frame
void AConsumableSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

