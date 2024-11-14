// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayElements/WaterBucket.h"

#include "GameplayElements/MuddyGround.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AWaterBucket::AWaterBucket()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWaterBucket::BeginPlay()
{
	Super::BeginPlay();

	UActorComponent* ComponentToGet = GetComponentByClass(UStaticMeshComponent::StaticClass());

	TObjectPtr<UStaticMeshComponent> BucketMesh = Cast<UStaticMeshComponent>(ComponentToGet);

	if(BucketMesh != nullptr)
	{
		BucketMesh->OnComponentHit.AddDynamic(this, &AWaterBucket::ComponentHit);
	}
	
}

// Called every frame
void AWaterBucket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWaterBucket::ComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(IsFilled)
	{
		if(Hit.GetActor()->ActorHasTag("Player"))
		{
			FHitResult PlayerTraceHit;
			GetWorld()->LineTraceSingleByChannel(PlayerTraceHit,
												OtherActor->GetActorLocation(),
												OtherActor->GetActorLocation() + 1000,
												ECC_GameTraceChannel1
												);

			SpawnMuddyGround(PlayerTraceHit.Location, FRotator(0, 0, 0));
		}
		else if(Hit.GetActor()->ActorHasTag("Ground"))
		{
			SpawnMuddyGround(Hit.Location, FRotator(0, 0, 0));
		}
	}
}

void AWaterBucket::SpawnMuddyGround(FVector Location, FRotator Rotation)
{
	GetWorld()->SpawnActor<AMuddyGround>(Location, Rotation);
	IsFilled = false;
}

