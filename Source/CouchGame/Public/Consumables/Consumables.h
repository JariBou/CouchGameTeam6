// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickable.h"
#include "Consumables.generated.h"

UENUM(BlueprintType)
enum class ETypeOfChangingStat : uint8
{
	NONE = 0,
	DAMAGE = 1 UMETA(DisplayName = "Damage"),
	HEALTH = 2 UMETA(DisplayName = "Health")
};

UCLASS()
class COUCHGAME_API AConsumables : public APickable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AConsumables();

public:
	UPROPERTY(EditAnywhere)
	float ChangingStatValue = 0.f;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETypeOfChangingStat> ChangingStat = ETypeOfChangingStat::NONE;
};


