// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayElements/MuddyGround.h"

#include "Characters/SfCharacter.h"
#include "DynamicMesh/ColliderMesh.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AMuddyGround::AMuddyGround()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMuddyGround::BeginPlay()
{
	Super::BeginPlay();
	UActorComponent* ComponentToGet = GetComponentByClass(UStaticMeshComponent::StaticClass());

	TObjectPtr<UStaticMeshComponent> MuddyGroundMesh = Cast<UStaticMeshComponent>(ComponentToGet);

	if(MuddyGroundMesh != nullptr)
	{
		MuddyGroundMesh->OnComponentBeginOverlap.AddDynamic(this, &AMuddyGround::BeginOverlap);
		MuddyGroundMesh->OnComponentEndOverlap.AddDynamic(this, &AMuddyGround::EndOverlap);
	}
}

// Called every frame
void AMuddyGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(OverlappingActorsAndSpeedOnEnter.Num() > 0)
	{
		for (TTuple<TObjectPtr<ASfCharacter>, float> CharacterAndSpeed : OverlappingActorsAndSpeedOnEnter)
		{
			CharacterAndSpeed.Key->GetCharacterMovement()->MaxWalkSpeed = CharacterAndSpeed.Value * (SlowPercent / 100.0f);
		}
	}

	Duration -= DeltaTime;

	if(Duration <= 0.0f)
	{
		Destroy();
	}
}
	
void AMuddyGround::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASfCharacter* Character = Cast<ASfCharacter>(OtherActor);

	if(Character == nullptr) return;

	OverlappingActorsAndSpeedOnEnter.Add(Character, Character->GetCharacterMovement()->MaxWalkSpeed);
}

void AMuddyGround::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASfCharacter* Character = Cast<ASfCharacter>(OtherActor);

	if(Character == nullptr) return;

	Character->GetCharacterMovement()->MaxWalkSpeed = OverlappingActorsAndSpeedOnEnter.FindRef(Character);

	OverlappingActorsAndSpeedOnEnter.Remove(Character);
}



