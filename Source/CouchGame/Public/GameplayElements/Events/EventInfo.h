// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
// #include "GameplayElements/Events/EventActor.h"
#include "EventInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct COUCHGAME_API FEventInfo : public FTableRowBase
{
	GENERATED_BODY()
		 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EventDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AEventActor> EventBp;
};
