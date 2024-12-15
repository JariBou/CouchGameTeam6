// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SfCharacter.h"
#include "GameFramework/Actor.h"
#include "MuddyGround.generated.h"

UCLASS(config=Game)
class ROYALRIOT_API AMuddyGround : public AActor
{
	GENERATED_BODY()
#pragma region Unreal Default
public:
	// Sets default values for this actor's properties
	AMuddyGround();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma endregion

#pragma region Mud

public:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	bool RemoveActorDebuff(AActor* OtherActor);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void BeginDestroy() override;
	
protected:
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UStaticMesh>> StaticMeshArray;
	
	UPROPERTY(EditAnywhere)
	float SlowPercent = 0.f;

	UPROPERTY(EditAnywhere)
	float DashDistancePercent = 0.f;

	UPROPERTY(EditAnywhere)
	float Duration = 0.f;

	UPROPERTY(EditAnywhere)
	float SizeMin = 0.f;

	UPROPERTY(EditAnywhere)
	float SizeMax = 0.f;

public:
	void SetDuration(float inDuration);

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
private:

	TMap<TObjectPtr<ASfCharacter>, float> OverlappingActorsAndSpeedOnEnter;
	
	TMap<TObjectPtr<ASfCharacter>, float> OverlappingActorsAndDashDistancedOnEnter;
	
#pragma endregion

#pragma region Vibrations Feedback

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UForceFeedbackEffect> Vibrations;

	UPROPERTY(EditAnywhere)
	FName VibrationTag;
	
#pragma endregion
};
