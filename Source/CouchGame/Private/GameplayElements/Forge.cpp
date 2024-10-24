﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayElements/Forge.h"


// Sets default values
AForge::AForge()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AForge::SpawnWeaponsAtRandomLocation(int NumberToSpawn, int SpawnDelayOfAttempts)
{
}

// Called when the game starts or when spawned
void AForge::BeginPlay()
{
	Super::BeginPlay();
	
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

