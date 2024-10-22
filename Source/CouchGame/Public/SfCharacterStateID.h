// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ESfCharacterStateID : uint8
{
	None = 0,
	Idle = 1,
	Walk = 2,
	Run = 3,
	Jump = 4,
	Fall = 5,
};