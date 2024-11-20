// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayElements/Events/VisualEventHandler.h"


// Sets default values for this component's properties
UVisualEventHandler::UVisualEventHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVisualEventHandler::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UVisualEventHandler::InformEndEvent()
{
	Super::InformEndEvent();

	FTimerHandle NullTimerHandle;
	GetTimeManager().SetTimer(NullTimerHandle, this, &UVisualEventHandler::StartVisualEffect, StaticTimeBeforeNextEvent - VisualEffectOffsetTime);

}

void UVisualEventHandler::StartVisualEffect()
{
	PlayVisualFeedback();
	FTimerHandle NullTimerHandle;
	GetTimeManager().SetTimer(NullTimerHandle, this, &UVisualEventHandler::EndVisualEffect, VisualEffectOffsetTime);
}

void UVisualEventHandler::EndVisualEffect()
{
	StopVisualFeedback();
	PlaySpawningVisualEffect();
}


// Called every frame
void UVisualEventHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

