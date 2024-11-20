// Fill out your copyright notice in the Description page of Project Settings.


#include "Consumables/Consumable.h"


// Sets default values
AConsumable::AConsumable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AConsumable::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AConsumable::EffectOnPlayer);
}

void AConsumable::EffectOnPlayer(UPrimitiveComponent* Comp, AActor* Char, UPrimitiveComponent* Comp2,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

// Called every frame
void AConsumable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

