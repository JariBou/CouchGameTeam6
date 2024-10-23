// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactions.h"
#include "TypeOfPickable.h"
#include "Components/StaticMeshComponent.h"
#include "Pickable.generated.h"


UCLASS()
class COUCHGAME_API APickable : public AActor, public IInteractions
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Interact_Implementation() override;

	virtual bool CanPickUp_Implementation(ASfCharacter* CouchGameCharacter) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Bitmask, BitmaskEnum = ETypeOfPickable))
	int32 TypeOfPickable = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;
};
