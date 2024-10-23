// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapons.generated.h"

class ASfCharacter;

USTRUCT()
struct FWeaponStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	uint8 Durability;

	UPROPERTY(EditAnywhere)
	uint32 Damage;

	UPROPERTY(EditAnywhere)
	uint8 DMGMultiplier;
};

USTRUCT()
struct FWeaponInfo {
	GENERATED_BODY()
		 
	UPROPERTY(EditAnywhere)
	FString WeaponName;

	UPROPERTY(EditAnywhere)
	FString ID_Name;
		 
	UPROPERTY(EditAnywhere)
	UStaticMesh* WeaponMesh;
		 
	//UPROPERTY(EditAnywhere)
	//TSubclassOf<ASfCharacter> WeaponBP;

	UPROPERTY(EditAnywhere)
	FWeaponStats WeaponStats;
};

//Weapons Data Asset
UCLASS()
class COUCHGAME_API UWeapons : public UPrimaryDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FWeaponInfo> WeaponsList;
};
