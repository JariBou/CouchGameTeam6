// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArrayUtils.generated.h"

/**
 * 
 */
UCLASS()
class COUCHGAME_API UArrayUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template<typename T>
	UFUNCTION(BlueprintCallable, Category = "ArrayUtils")
	static void GetRandomElement(const TArray<T>& Array, T& outElement);

	template<typename T>
	UFUNCTION(BlueprintCallable, Category = "ArrayUtils")
	static T* GetRandomElement(const TArray<T>& Array);

	template<typename T>
	UFUNCTION(BlueprintCallable, Category = "ArrayUtils")
	static T* GetRandomElement(TArray<T>& Array);
};


#if CPP 
#include "ArrayUtils.inl"
#endif