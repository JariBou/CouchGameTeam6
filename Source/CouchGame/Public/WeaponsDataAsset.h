// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponsDataAsset.generated.h"
class ASfCharacter;



UENUM(BlueprintType)
enum EWeaponRarity : uint8
{
	Common = 0 ,
	Rare = 1,
	SuperRare = 2,
	SuperSuperRare = 3,
	LegendFuckingDary = 4,
};


USTRUCT()
struct FWeaponStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	uint8 Durability = 1;

	UPROPERTY(EditAnywhere)
	uint32 Damage = 1;

	UPROPERTY(EditAnywhere)
	uint8 DMGMultiplier = 1;
};

USTRUCT()
struct FWeaponInfo : public FTableRowBase 
{
	GENERATED_BODY()
		 
	UPROPERTY(EditAnywhere)
	FString WeaponName;

	UPROPERTY(EditAnywhere)
	FString ID_Name;
		 
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> WeaponMesh;
		 
	//UPROPERTY(EditAnywhere)
	//TSubclassOf<ASfCharacter> WeaponBP;

	UPROPERTY(EditAnywhere)
	FWeaponStats WeaponStats;
};

/*
//Weapons Data Asset
UCLASS()
class COUCHGAME_API UWeapons : public UPrimaryDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FWeaponInfo> WeaponsList;
};
*/
