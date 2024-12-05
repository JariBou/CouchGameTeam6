// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactions.h"
#include "Components/StaticMeshComponent.h"
#include "Pickable.generated.h"


class UNiagaraSystem;
class UNiagaraComponent;

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

	void Interact_Implementation(ASfCharacter* CouchGameCharacter) override;

	virtual bool CanPickUp_Implementation(ASfCharacter* CouchGameCharacter) override;

	virtual void NiagaraDropSystem_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Bitmask, BitmaskEnum = ETypeOfPickable))
	int32 PickableType = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ASfCharacter> Holder;

	UPROPERTY(EditAnywhere, Category="ParticlePart")
	TObjectPtr<UNiagaraSystem> NiagaraParticleDrop;

	UPROPERTY(VisibleAnywhere, Category="ParticlePart")
	TObjectPtr<UNiagaraComponent> NiagaraComponentForDrop;
	
};
