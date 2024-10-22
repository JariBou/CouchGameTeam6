// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SfCharacter.h"
#include "UObject/Interface.h"
#include "Interactions.generated.h"

class ACouchGameCharacter;
// This class does not need to be modified.
UINTERFACE(MinimalAPI,Blueprintable)
class UInteractions : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COUCHGAME_API IInteractions
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:	
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Test")
	void Interact();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Pickup")
	bool CanPickUp(ASfCharacter* CouchGameCharacter);
};

