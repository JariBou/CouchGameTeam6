// Fill out your copyright notice in the Description page of Project Settings.


#include "SplinePoolComponent.h"

#include "Weapon.h"
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

TBitsToSizeType<32>::Type USplinePoolComponent::CreateNewSpline(const FVector& StartLocation, const FVector& EndLocation)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Spline Created");
	FTransform RelativeTransform = FTransform();
	USplineComponent* NewSplineComponent = Cast<USplineComponent>(GetOwner()->AddComponentByClass(USplineComponent::StaticClass(), false, RelativeTransform, true));
	
	NewSplineComponent->bAutoRegister = true;
	
	GetOwner()->FinishAddComponent(NewSplineComponent, false, RelativeTransform);

	FForgeSpline ForgeSpline;
	ForgeSpline.SplineComponent = NewSplineComponent;
	ForgeSpline.Destination = EndLocation;
	
	NewSplineComponent->ClearSplinePoints();
	FVector Vector = (EndLocation - StartLocation) * 1/4 + FVector(0, 0, 1000) + StartLocation;
	FVector Vector2 = (EndLocation - StartLocation) * 3/4 + FVector(0, 0, 1000) + StartLocation;

	NewSplineComponent->AddSplinePoint(StartLocation, ESplineCoordinateSpace::World, false);
	NewSplineComponent->AddSplinePoint(Vector, ESplineCoordinateSpace::World, false);
	NewSplineComponent->AddSplinePoint(Vector2, ESplineCoordinateSpace::World, false);
	NewSplineComponent->AddSplinePoint(EndLocation, ESplineCoordinateSpace::World, false);

	NewSplineComponent->UpdateSpline();

	NewSplineComponent->SetDrawDebug(true);
	
	return Splines.Add(ForgeSpline);
}

void USplinePoolComponent::StartSplineForWeapon(AWeapon* ForWeapon, const FVector& FromLocation, const FVector& ToLocation)
{
	int SplineIndex = -1;
	for (int i = 0; i < Splines.Num(); i++)
	{
		if ((Splines[i].Destination - ToLocation).SquaredLength() < 10)
		{
			// Do stuff
			SplineIndex = i;
			break;
		}
	}
	if (SplineIndex == -1)
	{
		SplineIndex = CreateNewSpline(FromLocation, ToLocation);
	}

	WeaponSplineIndexMap.Add(ForWeapon, SplineIndex);


	// Ballistics Tests ===================================================================================

	FVector TossVelocity;

	FCollisionResponseContainer Container;
	Container.SetAllChannels(ECR_Ignore);
	Container.SetResponse(ECC_WorldStatic, ECR_Block);
	
	UGameplayStatics::SuggestProjectileVelocity(GetWorld(), TossVelocity, FromLocation, ToLocation, 1000, true, 0, 1.f, ESuggestProjVelocityTraceOption::TraceFullPath, Container, {GetOwner()}, true);

	FPredictProjectilePathParams PredictParams;
	PredictParams.LaunchVelocity = TossVelocity;
	PredictParams.StartLocation = FromLocation;
	PredictParams.OverrideGravityZ = 1.f;
	PredictParams.ActorsToIgnore = {GetOwner()};
	PredictParams.DrawDebugType = EDrawDebugTrace::None;
	PredictParams.DrawDebugTime = 3;
	PredictParams.MaxSimTime = 3;


	FPredictProjectilePathResult PredictResult;
	UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);
	
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TossVelocity.ToString());
	
	
}

// Called when the game starts
void USplinePoolComponent::BeginPlay()
{
	Super::BeginPlay();

	//FTimerHandle Handle;
	//GetOwner()->GetWorldTimerManager().SetTimer(Handle, this, &USplinePoolComponent::TempFunc, 2, true);

	// ...
	
}

void USplinePoolComponent::TempFunc()
{
	CreateNewSpline(FVector(), GetOwner()->GetTransform().GetLocation());
}


// Called every frame
void USplinePoolComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TArray<AWeapon*> SplineKeys;
	WeaponSplineIndexMap.GetKeys(SplineKeys);
	
	for (int i = 0; i < SplineKeys.Num(); ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			AWeapon* Key = SplineKeys[i];
            int Value = WeaponSplineIndexMap[Key];
            
            TObjectPtr<USplineComponent> SplineComponent = Splines[Value].SplineComponent;
    
            // FVector ClosestToWorldLocation = SplineComponent->FindLocationClosestToWorldLocation(Pair.Key->GetTransform().GetLocation(), ESplineCoordinateSpace::World);
            FSplinePoint SplinePointAt = SplineComponent->GetSplinePointAt(SplineComponent->GetNumberOfSplinePoints(), ESplineCoordinateSpace::Local);
            // (GetOwner()->GetActorLocation() + SplinePointAt.Position)   //SplinePointAt.Position returns a local pos SMH 
            //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, (GetOwner()->GetActorLocation() + SplinePointAt.Position).ToString());
    
            // DOES NOT WORK (nvm fuck unreal it works)
            if ((Key->GetActorLocation() - (GetOwner()->GetActorLocation() + SplinePointAt.Position)).SquaredLength() < 100*100)
            {
            	WeaponSplineIndexMap.Remove(Key);
            	break;
            }
    
            FVector Direction = SplineComponent->FindDirectionClosestToWorldLocation(Key->GetTransform().GetLocation(), ESplineCoordinateSpace::World);
            Key->AddActorWorldOffset(Direction);
		}
		
	}
	
	// for (TTuple<AWeapon*, int> Pair : WeaponSplineIndexMap)
	// {
	// 	TObjectPtr<USplineComponent> SplineComponent = Splines[Pair.Value].SplineComponent;
	//
	// 	// FVector ClosestToWorldLocation = SplineComponent->FindLocationClosestToWorldLocation(Pair.Key->GetTransform().GetLocation(), ESplineCoordinateSpace::World);
	// 	FSplinePoint SplinePointAt = SplineComponent->GetSplinePointAt(SplineComponent->GetNumberOfSplinePoints(), ESplineCoordinateSpace::Local);
	// 	// (GetOwner()->GetActorLocation() + SplinePointAt.Position)   //SplinePointAt.Position returns a local pos SMH 
	// 	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, (GetOwner()->GetActorLocation() + SplinePointAt.Position).ToString());
	// 	
	// 	if ((Pair.Key->GetActorLocation() - SplinePointAt.Position).SquaredLength() < 100.f*100.f)
	// 	{
	// 		WeaponSplineIndexMap.Remove(Pair.Key);
	// 		continue;
	// 	}
	//
	// 	FVector Direction = SplineComponent->FindDirectionClosestToWorldLocation(Pair.Key->GetTransform().GetLocation(), ESplineCoordinateSpace::World);
	// 	Pair.Key->AddActorWorldOffset(Direction);
	// }

	// ...
}

