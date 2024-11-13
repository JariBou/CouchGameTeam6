// Fill out your copyright notice in the Description page of Project Settings.


#include "Consumables/Steak.h"


// Sets default values
ASteak::ASteak()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChangingStat = ETypeOfChangingStat::HEALTH;
	ChangingStatValue = 10.f;
}

// Called when the game starts or when spawned
void ASteak::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASteak::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

