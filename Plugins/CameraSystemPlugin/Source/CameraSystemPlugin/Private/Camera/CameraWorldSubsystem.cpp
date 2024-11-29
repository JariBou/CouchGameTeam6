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
	if (CameraMain == nullptr) return;
	CameraPluginSettings = GetDefault<UCameraPluginSettings>();
	
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
	float GreatestDistanceBetweenTargets = CalculateGreatestYDistanceBetweenTargets();

	float InvLerp = FMath::GetRangePct(CameraPluginSettings->CameraZoomDistanceBetweenTargetsMin, CameraPluginSettings->CameraZoomDistanceBetweenTargetsMax, GreatestDistanceBetweenTargets);
	InvLerp = FMath::Clamp(InvLerp, 0.0f, 1.0f);

	FVector newPos = FMath::Lerp(CameraZoomYMinLoc, CameraZoomYMaxLoc, InvLerp);
	
	CameraMain->GetOwner()->SetActorLocation(newPos);
}

void UCameraWorldSubsystem::TickUpdateCameraRotation(float DeltaTime)
{
	FVector AverageLocation = CalculateAveragePositionBetweenTargets();

	FVector DirDistance = AverageLocation - CameraMain->GetOwner()->GetActorLocation();

	FVector LerpedOffsetDirDistance = FMath::Lerp(StartForwardVector * DirDistance.Size(), DirDistance, CameraPluginSettings->CameraRotationMaxOffsetAlpha);
	
	FRotator NewRotation = LerpedOffsetDirDistance.Rotation();

	FRotator NewCamRotation = FMath::Lerp(CameraMain->GetOwner()->GetActorRotation(), NewRotation, CameraPluginSettings->CameraRotationSpeedAlpha);

	CameraMain->GetOwner()->SetActorRotation(NewCamRotation);
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

float UCameraWorldSubsystem::CalculateGreatestYDistanceBetweenTargets()
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
			
			float Distance = FMath::Abs(iCameraFollowTargetInterface->GetFollowTarget().Y - jCameraFollowTargetInterface->GetFollowTarget().Y);
			if (Distance > GreatestDistance) GreatestDistance = Distance;
		}
	}
	return GreatestDistance;
}

void UCameraWorldSubsystem::InitCameraRotationParameters()
{
	StartForwardVector = CameraMain->GetForwardVector();
}

void UCameraWorldSubsystem::InitCameraZoomParameters()
{
	UCameraComponent* CameraMin = FindCameraByTag("CameraDistanceMin");
	if (CameraMin != nullptr) CameraZoomYMinLoc = CameraMin->GetOwner()->GetActorLocation(); 
	
	UCameraComponent* CameraMax = FindCameraByTag("CameraDistanceMax");
	if (CameraMax != nullptr) CameraZoomYMaxLoc = CameraMax->GetOwner()->GetActorLocation();
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
	if (CameraMain == nullptr) return;
	Super::Tick(DeltaTime);
	TickUpdateCameraZoom(DeltaTime);
	TickUpdateCameraRotation(DeltaTime);
}
