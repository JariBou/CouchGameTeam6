// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventInfo.h"
#include "GameFramework/Actor.h"
#include "EventActor.generated.h"

class UEventHandler;

UCLASS()
class COUCHGAME_API AEventActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEventActor();

	virtual void EndEvent();
	
	void Config(UEventHandler* InEventHandler, const FEventInfo InEventInfo);

	void StartEvent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	TObjectPtr<UEventHandler> EventHandler;

	UPROPERTY()
	FEventInfo EventInfo;
};
