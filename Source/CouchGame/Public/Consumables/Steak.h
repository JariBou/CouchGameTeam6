#pragma once

#include "CoreMinimal.h"
#include "Consumable.h"
#include "Steak.generated.h"

UCLASS()
class COUCHGAME_API ASteak : public AConsumable
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

	UFUNCTION()
	virtual void EffectOnPlayer(UPrimitiveComponent* Comp, AActor* Char, UPrimitiveComponent* Comp2,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	UPROPERTY(EditAnywhere, meta=(UIMin = "0.0", UIMax = "100.0" ))
	uint8 HealthToAdd = 0;
};
