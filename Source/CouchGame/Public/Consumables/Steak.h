// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Consumables.h"
#include "Steak.generated.h"

UCLASS()
class COUCHGAME_API ASteak : public AConsumables
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASteak();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
