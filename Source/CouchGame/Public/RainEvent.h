// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayElements/Events/EventActor.h"
#include "RainEvent.generated.h"

class AMuddyGround;

UCLASS(Blueprintable, BlueprintType)
class COUCHGAME_API ARainEvent : public AEventActor
{
public:
	virtual void EndEvent() override;

private:
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARainEvent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMuddyGround> MuddyClass;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SpawnMuddyGround(FVector Location, const FRotator& Rotation, const FVector& NormalVector) const;
};
