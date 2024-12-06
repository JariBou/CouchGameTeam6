// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TransformUtils.generated.h"

/**
 * 
 */
UCLASS()
class ROYALRIOT_API UTransformUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TransformUtils")
	static FTransform MakeTransformFromLocation(const FVector& Location){
		FTransform Transform;
		Transform.SetLocation(Location);
		return Transform;
	}
};


// #if CPP
// #include "TransformUtils.inl"
// #endif