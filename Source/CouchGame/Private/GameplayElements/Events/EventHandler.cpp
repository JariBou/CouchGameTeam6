// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayElements/Events/EventHandler.h"

#include "GameplayElements/Events/EventInfo.h"
#include "GameplayElements/Events/EventActor.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/ArrayUtils.h"
#include "Utils/TransformUtils.h"

void UEventHandler::InitializeSpawnPoints()
{
	while(SpawnPointActors.Num() > 0)
	{
		const AActor* Element = SpawnPointActors[0];
		SpawnPoints.Add(Element->GetActorLocation());
		
		SpawnPointActors.RemoveAt(0);
	}
	// for (const AActor* Element : SpawnPointActors)
	// {
	// 	SpawnPoints.Add(Element->GetActorLocation());
	// }
	SpawnPointActors.Empty();
}

void UEventHandler::BeginPlay()
{
	Super::BeginPlay();
	InitializeSpawnPoints();
	
	FTimerHandle NullTimerHandle;
	GetTimeManager().SetTimer(NullTimerHandle, this, &UEventHandler::StartNewEvent, InitialDelay);
}

void UEventHandler::StartNewEvent()
{
	if (EventsDataTable == nullptr) return;
	TArray<FName> RowNames = EventsDataTable->GetRowNames();
	if (RowNames.Num() == 0) return;
	int32 Rand = 0;
	if (RowNames.Num() != 1)
	{
		do
		{
			Rand = FMath::RandRange(0,RowNames.Num());

		}
		while (RowNames[Rand] == LastEventName);
	}

	FName EventToSpawnName = RowNames[Rand];
	SpawnEvent(EventToSpawnName);
}

FTimerManager& UEventHandler::GetTimeManager() const
{
	return GetOwner()->GetGameInstance()->GetTimerManager();
}

void UEventHandler::InformEndEvent()
{
	int32 Rand = FMath::RandRange(static_cast<int32>(RandomAddedTimeRange.X),static_cast<int32>(RandomAddedTimeRange.Y));

	FTimerHandle NullTimerHandle;
	StaticTimeBeforeNextEvent = WaitTimeBetweenEvents + Rand;
	GetTimeManager().SetTimer(NullTimerHandle, this, &UEventHandler::StartNewEvent, StaticTimeBeforeNextEvent);
}

void UEventHandler::SpawnEvent(const FName EventName)
{
	const FEventInfo* EventInfo = EventsDataTable->FindRow<FEventInfo>(EventName, "");
	LastEventName = EventName;

	FVector spawnLocation;
	UArrayUtils::GetRandomElement(SpawnPoints, spawnLocation);
	FTransform SpawnTransform = UTransformUtils::MakeTransformFromLocation(spawnLocation);

	AEventActor* SpawnedEvent = Cast<AEventActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), EventInfo->EventBp, SpawnTransform));
	SpawnedEvent->Config(this, *EventInfo);

	SpawnedEvent->FinishSpawning(SpawnTransform);

	SpawnedEvent->StartEvent();
}
