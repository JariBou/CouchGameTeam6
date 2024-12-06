// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnPointToolUser.h"
#include "Components/ActorComponent.h"
#include "EventHandler.generated.h"

class AEventActor;
/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROYALRIOT_API UEventHandler : public UActorComponent, public ISpawnPointToolUser
{
	GENERATED_BODY()

	virtual TArray<AActor*> GetAllSpawnPoints_Implementation() override;

	virtual void SetSpawnPoints_Implementation(const TArray<AActor*>& NewRespawnPoints) override;

	virtual void AddSpawnPoint_Implementation(AActor* NewSpawnPoint) override;

private:
	UFUNCTION(CallInEditor)
	void InitializeSpawnPoints();

public:
	virtual void BeginPlay() override;
	
	virtual void StartNewEvent();

	virtual void InformEndEvent();

	UFUNCTION(BlueprintCallable)
	virtual void SpawnEvent(FName EventName);

	UPROPERTY(EditAnywhere)
	TArray<AActor*> SpawnPointActors;

protected:
	UPROPERTY()
	TObjectPtr<AEventActor> CurrentEvent;
	
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

	UPROPERTY()
	TArray<FVector> SpawnPoints;
};
