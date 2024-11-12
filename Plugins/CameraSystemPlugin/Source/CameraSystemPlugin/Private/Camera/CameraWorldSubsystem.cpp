// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraWorldSubsystem.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraFollowTarget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UnrealClient.h"
#include "Engine/GameViewportClient.h"
#include "CameraPluginSettings.h"


void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	CameraMain = FindCameraByTag(TEXT("CameraMain"));
	CameraPluginSettings = GetDefault<UCameraPluginSettings>();

	AActor* CameraBoundsActor = FindCameraBoundsActor();
	if (CameraBoundsActor != nullptr) InitCameraBounds(CameraBoundsActor);

	InitCameraRotationParameters();
	InitCameraZoomParameters();
}

void UCameraWorldSubsystem::AddFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Add(FollowTarget);
}

void UCameraWorldSubsystem::RemoveFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Remove(FollowTarget);
}

void UCameraWorldSubsystem::TickUpdateCameraZoom(float DeltaTime)
{
	if (CameraMain == nullptr) return;
	float GreatestDistanceBetweenTargets = CalculateGreatestDistanceBetweenTargets();

	float InvLerp = FMath::GetRangePct(CameraPluginSettings->CameraZoomDistanceBetweenTargetsMin, CameraPluginSettings->CameraZoomDistanceBetweenTargetsMax, GreatestDistanceBetweenTargets);
	InvLerp = FMath::Clamp(InvLerp, 0.0f, 1.0f);
	
	FVector newPos = CameraMain->GetOwner()->GetActorLocation();
	//newPos.Y = FMath::Lerp(CameraZoomYMax, CameraZoomYMin, InvLerp);
	newPos = FMath::Lerp(CameraZoomYMinLoc, CameraZoomYMaxLoc, InvLerp);
	// newPos.X = FMath::Lerp(CameraZoomYMaxLoc.X, CameraZoomYMinLoc.X, InvLerp);
	// newPos.Y = FMath::Lerp(CameraZoomYMaxLoc.Y, CameraZoomYMinLoc.Y, InvLerp);
	// newPos.Z = FMath::Lerp(CameraZoomYMaxLoc.Z, CameraZoomYMinLoc.Z, InvLerp);
	
	CameraMain->GetOwner()->SetActorLocation(newPos);
}

void UCameraWorldSubsystem::TickUpdateCameraRotation(float DeltaTime)
{
	FVector AverageLocation = CalculateAveragePositionBetweenTargets();

	FVector DirDistance = AverageLocation - CameraMain->GetOwner()->GetActorLocation();

	FVector LerpedOffsetDirDistance = FMath::Lerp(StartForwardVector * DirDistance.Size(), DirDistance, .3f);
	
	FRotator NewRotation = LerpedOffsetDirDistance.Rotation();

	FRotator NewCamRotation = FMath::Lerp(CameraMain->GetOwner()->GetActorRotation(), NewRotation, .3f);

	CameraMain->GetOwner()->SetActorRotation(NewCamRotation);
}

void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	FVector AverageLocation = CalculateAveragePositionBetweenTargets();
	//AverageLocation.Y = CameraMain->GetComponentLocation().Y;
	ClampPositionIntoCameraBounds(AverageLocation);

	FVector newPos = AverageLocation;
	newPos.Y = CameraMain->GetOwner()->GetActorLocation().Y;
	
	CameraMain->GetOwner()->SetActorLocation(newPos);
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets()
{
	FVector averagePos;
	for (UObject* FollowTarget : FollowTargets)
	{
		TScriptInterface<ICameraFollowTarget> CameraFollowTargetInterface = FollowTarget;
		if (CameraFollowTargetInterface == nullptr) continue;
		if (CameraFollowTargetInterface->IsFollowable())
		{
			averagePos += CameraFollowTargetInterface->GetFollowTarget();
		}
	}
	averagePos /= FollowTargets.Num();
	return averagePos;
}

float UCameraWorldSubsystem::CalculateGreatestDistanceBetweenTargets()
{
	float GreatestDistance = 0.f;
	
	for (int i = 0; i < FollowTargets.Num(); ++i)
	{
		TScriptInterface<ICameraFollowTarget> iCameraFollowTargetInterface = FollowTargets[i];
		if (iCameraFollowTargetInterface == nullptr) continue;
		
		for (int j = i+1; j < FollowTargets.Num(); ++j)
		{
			TScriptInterface<ICameraFollowTarget> jCameraFollowTargetInterface = FollowTargets[j];
			if (jCameraFollowTargetInterface == nullptr) continue;
			
			//float SizeSquared = (iCameraFollowTargetInterface->GetFollowTarget() - jCameraFollowTargetInterface->GetFollowTarget()).SizeSquared();
			float Distance = FMath::Abs(iCameraFollowTargetInterface->GetFollowTarget().Y - jCameraFollowTargetInterface->GetFollowTarget().Y);
			if (Distance > GreatestDistance) GreatestDistance = Distance;
		}
	}
	// return FMath::Sqrt(GreatestDistanceSqrd);
	return GreatestDistance;
}

void UCameraWorldSubsystem::InitCameraRotationParameters()
{
	StartForwardVector = CameraMain->GetForwardVector();
}

void UCameraWorldSubsystem::InitCameraZoomParameters()
{
	UCameraComponent* CameraMin = FindCameraByTag("CameraDistanceMin");
	//if (CameraMin != nullptr) CameraZoomYMin = CameraMin->GetOwner()->GetActorLocation().Y; 
	if (CameraMin != nullptr) CameraZoomYMinLoc = CameraMin->GetOwner()->GetActorLocation(); 
	
	UCameraComponent* CameraMax = FindCameraByTag("CameraDistanceMax");
	//if (CameraMax != nullptr) CameraZoomYMax = CameraMax->GetOwner()->GetActorLocation().Y; 
	if (CameraMax != nullptr) CameraZoomYMaxLoc = CameraMax->GetOwner()->GetActorLocation();
}

AActor* UCameraWorldSubsystem::FindCameraBoundsActor()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "CameraBounds", FoundActors);

	if (FoundActors.Num() > 0) return FoundActors[0];
	return nullptr;
}

void UCameraWorldSubsystem::InitCameraBounds(AActor* CameraBoundsActor)
{
	FVector BoundsCenter;
	FVector BoundsExtents;
	CameraBoundsActor->GetActorBounds(false, BoundsCenter, BoundsExtents);

	CameraBoundsMin = FVector2D(BoundsCenter.X - BoundsExtents.X, BoundsCenter.Z - BoundsExtents.Z);
	CameraBoundsMax = FVector2D(BoundsCenter.X + BoundsExtents.X, BoundsCenter.Z + BoundsExtents.Z);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("CameraBoundsMin: X=%f, Z=%f"), CameraBoundsMin.X, CameraBoundsMin.Y));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("CameraBoundsMin: X=%f, Z=%f"), CameraBoundsMax.X, CameraBoundsMax.Y));


	CameraBoundsYProjectionCenter = BoundsCenter.Y;
}

void UCameraWorldSubsystem::ClampPositionIntoCameraBounds(FVector& Position)
{
	FVector2D ViewportBoundsMin, ViewportBoundsMax;
	GetViewportBounds(ViewportBoundsMin, ViewportBoundsMax);

	FVector WorldBoundsMin = CalculateWorldPositionFromViewportPosition(ViewportBoundsMin);
	FVector WorldBoundsMax = CalculateWorldPositionFromViewportPosition(ViewportBoundsMax);

	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("WorldBoundsMin: X=%f, Z=%f"), WorldBoundsMin.X, WorldBoundsMin.Z));
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("WorldBoundsMax: X=%f, Z=%f"), WorldBoundsMax.X, WorldBoundsMax.Z));

	FVector WorldBoundsRange = WorldBoundsMax - WorldBoundsMin;
	FVector WorldBoundsExtents = WorldBoundsRange / 2.f;

	Position.X = FMath::Clamp(Position.X, CameraBoundsMin.X + WorldBoundsExtents.X, CameraBoundsMax.X - WorldBoundsExtents.X);
	Position.Z = FMath::Clamp(Position.Z, CameraBoundsMin.Y - WorldBoundsExtents.Z, CameraBoundsMax.Y + WorldBoundsExtents.Z);
}

void UCameraWorldSubsystem::GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax)
{

	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if (ViewportClient == nullptr) return;

	FViewport* Viewport = ViewportClient->Viewport;
	if (Viewport == nullptr) return;

	FIntRect ViewRect(
		Viewport->GetInitialPositionXY(),
		Viewport->GetInitialPositionXY() + Viewport->GetSizeXY()
		);
	FIntRect ViewportRect = Viewport->CalculateViewExtents(CameraMain->AspectRatio, ViewRect);

	OutViewportBoundsMin.X = ViewportRect.Min.X;
	OutViewportBoundsMin.Y = ViewportRect.Min.Y;
	
	OutViewportBoundsMax.X = ViewportRect.Max.X;
	OutViewportBoundsMax.Y = ViewportRect.Max.Y;
	
}

FVector UCameraWorldSubsystem::CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition)
{
	if (CameraMain == nullptr) return FVector::Zero();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController == nullptr) return FVector::Zero();

	float yDistanceToCenter = CameraMain->GetOwner()->GetActorLocation().Y - CameraBoundsYProjectionCenter;

	FVector CameraWorldProjectDir;
	FVector WorldPosition;
	UGameplayStatics::DeprojectScreenToWorld(
		PlayerController,
		ViewportPosition,
		WorldPosition,
		CameraWorldProjectDir);

	WorldPosition += CameraWorldProjectDir * yDistanceToCenter;
	
	return WorldPosition;
}

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag(const FName& Tag) const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

	
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		UActorComponent* Component = Actor->GetComponentByClass(UCameraComponent::StaticClass());
		UCameraComponent* CameraComponent = Cast<UCameraComponent>(Component);
		if (CameraComponent != nullptr)
		{
			return CameraComponent;
		}
	}
	return nullptr;
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickUpdateCameraZoom(DeltaTime);
	TickUpdateCameraRotation(DeltaTime);
	//TickUpdateCameraPosition(DeltaTime);
}
