// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickable.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/SfCharacter.h"
#include "NiagaraSystem.h"


// Sets default values
APickable::APickable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseStaticMesh"));
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

void APickable::Interact_Implementation(ASfCharacter* CouchGameCharacter)
{
	// GEngine->AddOnScreenDebugMessage(-1, 2 , FColor::Purple, TEXT("Interaction With Interface"));
	if (CouchGameCharacter != nullptr) Holder = CouchGameCharacter;
	NiagaraDropSystem_Implementation();
}

bool APickable::CanPickUp_Implementation(ASfCharacter* CouchGameCharacter)
{
	TEnumAsByte<TypeOfPlayer> PlayerType = CouchGameCharacter->PlayerType;
	return PlayerType & PickableType;
}

void APickable::NiagaraDropSystem_Implementation()
{
	NiagaraComponentForDrop = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraParticleDrop, StaticMeshComponent, NAME_None, FVector(0.f,0.f,0.f), FRotator(0.f), EAttachLocation::Type::SnapToTarget, true);
}

