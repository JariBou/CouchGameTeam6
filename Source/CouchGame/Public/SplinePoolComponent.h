// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SplinePoolComponent.generated.h"

class AWeapon;
class USplineComponent;

USTRUCT(BlueprintType)
struct FForgeSpline
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USplineComponent> SplineComponent;

	UPROPERTY()
	bool IsFree = true;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COUCHGAME_API USplinePoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USplinePoolComponent();

	TBitsToSizeType<32>::Type CreateNewSpline(FVector EndLocation);

	void StartSplineForWeapon(AWeapon* ForWeapon, FVector FromLocation, FVector ToLocation);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void TempFunc();

	UPROPERTY(EditAnywhere)
	TArray<FForgeSpline> Splines;

	UPROPERTY(VisibleAnywhere)
	TMap<AWeapon*, int> WeaponSplineIndexMap;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
