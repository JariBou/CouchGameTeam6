// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayElements/Events/EventActor.h"

#include "GameplayElements/Events/EventHandler.h"


// Sets default values
AEventActor::AEventActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AEventActor::EndEvent()
{
	EventHandler->InformEndEvent();
}

void AEventActor::Config(UEventHandler* InEventHandler, const FEventInfo& InEventInfo)
{
	EventHandler = InEventHandler;
	EventInfo = InEventInfo;
}

void AEventActor::StartEvent()
{
	FTimerHandle NullTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(NullTimerHandle, this, &AEventActor::EndEvent, EventInfo.EventDuration);
}

// Called when the game starts or when spawned
void AEventActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEventActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

