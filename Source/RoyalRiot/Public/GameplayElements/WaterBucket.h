// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickable.h"
#include "WaterBucket.generated.h"

class AMuddyGround;

UCLASS()
class ROYALRIOT_API AWaterBucket : public APickable
{
	GENERATED_BODY()

#pragma region Unreal Default
	
public:
	// Sets default values for this actor's properties
	AWaterBucket();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
		
#pragma endregion

#pragma region Bucket

public:
	UFUNCTION()
	void ComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable)
	void SwitchFillBucket();

	void UpdateMesh();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsFilled = false;

protected:
	UFUNCTION()
	void SpawnMuddyGround(FVector Location, const FRotator& Rotation, const FVector& NormalVector);

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMuddyGround> MuddyClass;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> FilledMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> EmptyMesh;
	
private:

#pragma endregion

#pragma region Sounds

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTriggerSound);

public:
	UPROPERTY(BlueprintAssignable, Category = "event")
	FTriggerSound TriggerFillSound;

	UPROPERTY(BlueprintAssignable, Category = "event")
	FTriggerSound TriggerHitGroundSound;

	UPROPERTY(BlueprintAssignable, Category = "event")
	FTriggerSound TriggerPickupSound;

	UPROPERTY(BlueprintAssignable, Category = "event")
	FTriggerSound TriggerThrowSound;

	
	
#pragma endregion
};
