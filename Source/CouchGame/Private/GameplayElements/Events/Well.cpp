// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayElements/Events/Well.h"


// Sets default values for this component's properties
UWell::UWell()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWell::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UWell::InformEndEvent()
{
	Super::InformEndEvent();

	FTimerHandle NullTimerHandle;
	GetTimeManager().SetTimer(NullTimerHandle, this, &UWell::StartVisualEffect, StaticTimeBeforeNextEvent - VisualEffectOffsetTime);

}

void UWell::StartVisualEffect()
{
	PlayVisualFeedback();
	FTimerHandle NullTimerHandle;
	GetTimeManager().SetTimer(NullTimerHandle, this, &UWell::EndVisualEffect, VisualEffectOffsetTime);
}

void UWell::EndVisualEffect()
{
	StopVisualFeedback();
	PlaySpawningVisualEffect();
}


// Called every frame
void UWell::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

