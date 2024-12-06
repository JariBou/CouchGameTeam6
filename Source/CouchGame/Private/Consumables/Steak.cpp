// Fill out your copyright notice in the Description page of Project Settings.


#include "Consumables/Steak.h"
#include "./Characters/SfCharacter.h"


// Sets default values
ASteak::ASteak()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ChangingStat = ETypeOfChangingStat::HEALTH;
	//Arnaud suce trop bien vrai dinguerie
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

void ASteak::EffectOnPlayer(UPrimitiveComponent* Comp, AActor* Char, UPrimitiveComponent* Comp2, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	Super::EffectOnPlayer(Comp, Char, Comp2, OtherBodyIndex, bFromSweep, SweepResult);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("OE"));

	ASfCharacter* Player = Cast<ASfCharacter>(Char);

	if (Player == nullptr) return;
	
	Player->AddHealth(HealthToAdd);
	Player->UsedHealingSource();

	this->Destroy();
}

void ASteak::GetConsumedBy(ASfCharacter* Consumer)
{
	Super::GetConsumedBy(Consumer);
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Consumed");
	if (Consumer == nullptr) return;
	Consumer->AddHealth(HealthToAdd);
	this->Destroy();
}


//ENORME CHIBRE DE LA PART DE CLEMENT https://yt3.googleusercontent.com/UrDr6Rp55gv3wpuyUimesOLOhrNqEgSV5h9rXU0n423n1fSjcyNDJ41yCg0ypL9GT7JKdRlck_w=s900-c-k-c0x00ffffff-no-rj

