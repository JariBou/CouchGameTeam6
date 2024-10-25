// Fill out your copyright notice in the Description page of Project Settings.


#include "SplinePoolComponent.h"

#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
USplinePoolComponent::USplinePoolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USplinePoolComponent::CreateNewSpline(FVector EndLocation)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Spline Created");
	FTransform RelativeTransform = FTransform();
	USplineComponent* NewSplineComponent = Cast<USplineComponent>(GetOwner()->AddComponentByClass(USplineComponent::StaticClass(), false, RelativeTransform, true));
	
	NewSplineComponent->bAutoRegister = true;
	
	GetOwner()->FinishAddComponent(NewSplineComponent, false, RelativeTransform);

	FForgeSpline ForgeSpline;
	ForgeSpline.SplineComponent = NewSplineComponent;
	Splines.Add(ForgeSpline);
	
	NewSplineComponent->ClearSplinePoints();
	FVector Vector = EndLocation * 1/4 + FVector(0, 0, 1000);
	FVector Vector2 = EndLocation * 3/4 + FVector(0, 0, 1000);

	NewSplineComponent->AddSplinePoint(FVector(), ESplineCoordinateSpace::World, false);
	NewSplineComponent->AddSplinePoint(Vector, ESplineCoordinateSpace::World, false);
	NewSplineComponent->AddSplinePoint(Vector2, ESplineCoordinateSpace::World, false);
	NewSplineComponent->AddSplinePoint(EndLocation, ESplineCoordinateSpace::World, false);

	NewSplineComponent->UpdateSpline();

	NewSplineComponent->SetDrawDebug(true);

	// FVector OutTossVelocity;
	// UGameplayStatics::SuggestProjectileVelocity(GetOwner()->GetWorld(), OutTossVelocity, FVector(), EndLocation, 100000, true);
	//
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, OutTossVelocity.ToString());
	//
	// FPredictProjectilePathParams PredictParams{
	// 2, FVector(), OutTossVelocity, 2};
	// PredictParams.TraceChannel = ECC_WorldStatic;
	// PredictParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	// PredictParams.DrawDebugTime = 1;
	//
	// FPredictProjectilePathResult PredictResults;
	// UGameplayStatics::PredictProjectilePath(GetOwner()->GetWorld(), PredictParams, PredictResults);
}


// Called when the game starts
void USplinePoolComponent::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle Handle;
	GetOwner()->GetWorldTimerManager().SetTimer(Handle, this, &USplinePoolComponent::TempFunc, 2, true);

	// ...
	
}

void USplinePoolComponent::TempFunc()
{
	CreateNewSpline(GetOwner()->GetTransform().GetLocation());
}


// Called every frame
void USplinePoolComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	

	// ...
}

