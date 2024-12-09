// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayElements/MuddyGround.h"

#include "Characters/SfCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AMuddyGround::AMuddyGround()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
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
			//Change speed to speed * slow value
			if(SlowPercent > 0.0f) CharacterAndSpeed.Key->GetCharacterMovement()->MaxWalkSpeed = CharacterAndSpeed.Value * (SlowPercent / 100.0f);

			//Change dash distance value to dash distance * dash percentage
			if(DashDistancePercent > 0.0f) CharacterAndSpeed.Key->DashDistance = OverlappingActorsAndDashDistancedOnEnter.FindRef(CharacterAndSpeed.Key) * (DashDistancePercent / 100.0f);
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
	if (OverlappingActorsAndSpeedOnEnter.Contains(Character)) return;
		
	Character->StartFeedBackEffect(true);

	//Add character in list of overlapping characters, character as key and speed as value
	OverlappingActorsAndSpeedOnEnter.Add(Character, Character->GetCharacterMovement()->MaxWalkSpeed);

	//Add character in list of overlapping characters, character as key and dash distance as value
	OverlappingActorsAndDashDistancedOnEnter.Add(Character, Character->DashDistance);
	
	// //Change speed to speed * slow value
	// if(SlowPercent > 0.0f) Character->GetCharacterMovement()->MaxWalkSpeed = Character->GetCharacterMovement()->MaxWalkSpeed * (SlowPercent / 100.0f);
	//
	// //Change dash distance value to dash distance * dash percentage
	// if(DashDistancePercent > 0.0f) Character->DashDistance = OverlappingActorsAndDashDistancedOnEnter.FindRef(Character) * (DashDistancePercent / 100.0f);
}

bool AMuddyGround::RemoveActorDebuff(AActor* OtherActor)
{
	ASfCharacter* Character = Cast<ASfCharacter>(OtherActor);

	if(Character == nullptr) return true;

	Character->StopFeedBackEffect();
	if (!OverlappingActorsAndSpeedOnEnter.Contains(Character)) return true;

	//Set character speed to its value before entering and remove it from overlapping characters list
	Character->GetCharacterMovement()->MaxWalkSpeed = OverlappingActorsAndSpeedOnEnter.FindRef(Character);
	OverlappingActorsAndSpeedOnEnter.Remove(Character);

	//Set character dash distance to its value before entering and remove it from overlapping characters list
	Character->DashDistance = OverlappingActorsAndDashDistancedOnEnter.FindRef(Character);
	OverlappingActorsAndDashDistancedOnEnter.Remove(Character);
	return false;
}

void AMuddyGround::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	RemoveActorDebuff(OtherActor);
}

void AMuddyGround::BeginDestroy()
{
	Super::BeginDestroy();
	// for (auto& [Actor, _] : OverlappingActorsAndSpeedOnEnter)
	// {
	// 	RemoveActorDebuff(Actor);
	// }
}

void AMuddyGround::SetDuration(float inDuration)
{
	Duration = inDuration;
}



