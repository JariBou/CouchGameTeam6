// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickable.h"
#include "AssetTypeCategories.h"
#include "Characters/SfCharacter.h"


// Sets default values
APickable::APickable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APickable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickable::Interact_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 2 , FColor::Blue, TEXT("Interaction With Interface"));
}

bool APickable::CanPickUp_Implementation(ASfCharacter* CouchGameCharacter)
{
	TEnumAsByte<TypeOfPlayer> PlayerType = CouchGameCharacter->PlayerType;
	return PlayerType & TypeOfPickable;
}

