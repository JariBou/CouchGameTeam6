// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EventHandler.generated.h"

class AEventActor;
/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COUCHGAME_API UEventHandler : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AEventActor> CurrentEvent;

public:
	virtual void BeginPlay() override;
	
	virtual void StartNewEvent();

	virtual void InformEndEvent();

	UFUNCTION(BlueprintCallable)
	virtual void SpawnEvent(FName EventName);

protected:
	FTimerManager& GetTimeManager() const;

	int32 StaticTimeBeforeNextEvent;

	FName LastEventName;
	
	UPROPERTY(EditAnywhere)
	int32 InitialDelay = 20;

	UPROPERTY(EditAnywhere)
	int32 WaitTimeBetweenEvents = 20;

	UPROPERTY(EditAnywhere)
	FVector2D RandomAddedTimeRange = FVector2D(1, 10);

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> EventsDataTable;
};
