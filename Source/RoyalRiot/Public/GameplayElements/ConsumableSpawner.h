// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Consumables/Consumable.h>

#include "CoreMinimal.h"
#include <GameFramework/Actor.h>
#include "ConsumableSpawner.generated.h"

UCLASS()
class ROYALRIOT_API AConsumableSpawner : public AActor
{
	GENERATED_BODY()

	public:
		// Sets default values for this actor's properties
		AConsumableSpawner();

	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

	UFUNCTION()
	void SpawnRandomConsumable();

	UFUNCTION()
	void OnSpawnedItemPickedUp();

	public:
		// Called every frame
		virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float InitialSpawnDelay;
	
	UPROPERTY(EditAnywhere)
	float SpawnDelay;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AConsumable>> PossibleConsumables;
	
	UPROPERTY()
	AConsumable* Consumable;
};
