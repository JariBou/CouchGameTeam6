// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventHandler.h"
#include "Well.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COUCHGAME_API UWell : public UEventHandler
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWell();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InformEndEvent() override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayVisualFeedback();
	
	UFUNCTION(BlueprintImplementableEvent)
	void StopVisualFeedback();

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySpawningVisualEffect();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:

	void StartVisualEffect();

	void EndVisualEffect();
	
	UPROPERTY(EditAnywhere)
	int32 VisualEffectOffsetTime;
};
