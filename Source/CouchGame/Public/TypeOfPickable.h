// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//Enum Class
UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum ETypeOfPickable : uint8
{
	NONE = 0 UMETA(Hidden),
	KNIGHT = 1 << 0,
	SQUIRE = 1 << 1,
};
ENUM_CLASS_FLAGS(ETypeOfPickable);

UENUM(BlueprintType)
enum TypeOfPlayer : uint8
{
	None = 0 UMETA(Hidden),
	Knight = 1 << 0,
	Squire = 1 << 1
};
