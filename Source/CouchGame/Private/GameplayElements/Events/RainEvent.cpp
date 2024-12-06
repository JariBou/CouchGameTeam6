// Fill out your copyright notice in the Description page of Project Settings.


#include <GameplayElements/Events/RainEvent.h>

#include "GameplayElements/MuddyGround.h"
#include "Kismet/GameplayStatics.h"


void ARainEvent::EndEvent()
{
	Super::EndEvent();

	GetWorld()->DestroyActor(this);
}

// Sets default values
ARainEvent::ARainEvent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARainEvent::BeginPlay()
{
	Super::BeginPlay();

	// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "ARainEvent::BeginPlay");
	
	SetActorLocation(GetActorLocation() + FVector(0, 0, 100));

	TArray<struct FHitResult> OutHits;
	FCollisionQueryParams Params;
#if UE_EDITOR
	Params.bDebugQuery = true;
#endif
	if (GetWorld()->LineTraceMultiByChannel(OutHits, GetActorLocation(), GetActorLocation() - FVector(0, 0, 100),
	                                        ECC_WorldStatic))
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), RainSound, GetActorLocation());
		SpawnMuddyGround(OutHits[0].Location, FRotator(0,0,0), OutHits[0].Normal);

	}
}

// Called every frame
void ARainEvent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARainEvent::SpawnMuddyGround(FVector Location, const FRotator& Rotation, const FVector& NormalVector) const
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Spawn Ground");
	AMuddyGround* SpawnedActor = GetWorld()->SpawnActor<AMuddyGround>(MuddyClass, Location, Rotation);
	SpawnedActor->SetActorLocation(SpawnedActor->GetActorLocation() + NormalVector * 5);

	SpawnedActor->SetDuration(EventInfo.EventDuration);
	
	FRotator NewRotation = FRotator(FMath::RadiansToDegrees(FMath::Atan2(NormalVector.Z, NormalVector.Y))-90,
		SpawnedActor->GetActorRotation().Vector().Z,
		FMath::RadiansToDegrees(FMath::Atan2(NormalVector.Z, NormalVector.X))-90);
	//GEngine->AddOnScreenDebugMessage(	-1, 5.f, FColor::Red, NewRotation.ToString()	);

	SpawnedActor->SetActorRotation(FRotator(0,0,0));
}

