// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayElements/WaterBucket.h"
#include "GameFramework/PlayerState.h"
#include "GameplayElements/MuddyGround.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


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

	//UActorComponent* ComponentToGet = GetComponentByClass(UStaticMeshComponent::StaticClass());

	//TObjectPtr<UStaticMeshComponent> BucketMesh = Cast<UStaticMeshComponent>(ComponentToGet);

	if(StaticMeshComponent != nullptr)
	{
		StaticMeshComponent->OnComponentHit.AddDynamic(this, &AWaterBucket::ComponentHit);
	}

	UpdateMesh();
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
		//TEST DE COLLIDE AVEC UN PLAYER ET FAIRE POP BOUE SOUS SES PIEDS
		/*if(Hit.GetActor()->ActorHasTag("Player") && Cast<ASfCharacter>(Hit.GetActor()) != Cast<AActor>(Holder))
		{
			FHitResult PlayerTraceHit;
			GetWorld()->LineTraceSingleByChannel(PlayerTraceHit,
												OtherActor->GetActorLocation(),
												OtherActor->GetActorLocation() + 1000,
												ECC_GameTraceChannel1
												);

			SpawnMuddyGround(PlayerTraceHit.Location, FRotator(0, 0, 0), FVector::UpVector);
		}
		else*/

		if(Hit.GetActor()->ActorHasTag("Ground"))
		{
			TriggerHitGroundSound.Broadcast();
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Water Bucket Hit Ground");
			SpawnMuddyGround(Hit.ImpactPoint, FRotator(0,0,0), Hit.GetActor()->GetActorUpVector());
		}
	}
}

void AWaterBucket::SwitchFillBucket()
{
	IsFilled = !IsFilled;

	UpdateMesh();
}

void AWaterBucket::UpdateMesh()
{
	if(IsFilled)
	{
		TriggerFillSound.Broadcast();
		StaticMeshComponent->SetStaticMesh(FilledMesh);
	}
	else
	{
		StaticMeshComponent->SetStaticMesh(EmptyMesh);
	}
}

void AWaterBucket::SpawnMuddyGround(FVector Location, const FRotator& Rotation, const FVector& NormalVector)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Spawn Ground");
	AActor* SpawnedActor = GetWorld()->SpawnActor<AMuddyGround>(MuddyClass, Location, Rotation);
	SpawnedActor->SetActorLocation(SpawnedActor->GetActorLocation() + NormalVector * 5);

	
	FRotator NewRotation = FRotator(FMath::RadiansToDegrees(FMath::Atan2(NormalVector.Z, NormalVector.Y))-90,
		SpawnedActor->GetActorRotation().Vector().Z,
		FMath::RadiansToDegrees(FMath::Atan2(NormalVector.Z, NormalVector.X))-90);
	//GEngine->AddOnScreenDebugMessage(	-1, 5.f, FColor::Red, NewRotation.ToString()	);

	SpawnedActor->SetActorRotation(FRotator(0,0,0));
	SwitchFillBucket();
}

