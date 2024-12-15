// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactions.h"
#include "Components/StaticMeshComponent.h"
#include "Pickable.generated.h"


class ANiagaraActor;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS(BlueprintType)
class ROYALRIOT_API APickable : public AActor, public IInteractions
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickable();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickedUp);

	UPROPERTY()
	FOnPickedUp OnPickedUp;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UUserWidget> FeedbackWidget;

	UFUNCTION(BlueprintCallable)
	void SetFeedbackWidget(UUserWidget* NewFeedbackWidget);

	UFUNCTION(BlueprintCallable)
	void DestroyPickable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(ASfCharacter* CouchGameCharacter) override;

	virtual bool CanPickUp_Implementation(ASfCharacter* CouchGameCharacter) override;

	virtual void NiagaraDropSystem_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Bitmask, BitmaskEnum = "/Script/RoyalRiot.ETypeOfPickable"))
	int32 PickableType = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Holder")
	TObjectPtr<ASfCharacter> Holder;

	UPROPERTY(EditAnywhere, Category="ParticlePart")
	TObjectPtr<UNiagaraSystem> NiagaraParticleDrop;

	UPROPERTY(VisibleAnywhere, Category="ParticlePart")
	TObjectPtr<UNiagaraComponent> NiagaraComponentForDrop;

	UPROPERTY(EditAnywhere,Category="ParticlePart")
	bool IsNiagaraOn = true;
};
