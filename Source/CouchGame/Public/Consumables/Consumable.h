// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickable.h"
#include "Consumable.generated.h"

UENUM(BlueprintType)
enum class ETypeOfChangingStat : uint8
{
	NONE = 0,
	DAMAGE = 1 UMETA(DisplayName = "Damage"),
	HEALTH = 2 UMETA(DisplayName = "Health")
};

UCLASS()
class COUCHGAME_API AConsumable : public APickable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AConsumable();

	UPROPERTY()
	ETypeOfChangingStat ChangingStat = ETypeOfChangingStat::NONE;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void EffectOnPlayer(UPrimitiveComponent* Comp, AActor* Char, UPrimitiveComponent* Comp2, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
