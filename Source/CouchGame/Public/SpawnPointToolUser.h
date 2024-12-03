// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpawnPointToolUser.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, BlueprintType)
class USpawnPointToolUser : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COUCHGAME_API ISpawnPointToolUser
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SpawnPointToolUser")
	TArray<AActor*> GetAllSpawnPoints();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SpawnPointToolUser")
	void SetSpawnPoints(const TArray<AActor*>& NewRespawnPoints);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SpawnPointToolUser")
	void AddSpawnPoint(AActor* NewSpawnPoint);

};
