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


USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Durability = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Damage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 DMGMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Lifetime;
};

USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase 
{
	GENERATED_BODY()
		 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ID_Name;
		 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> WeaponMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AWeapon> WeaponBP;
		 
	//UPROPERTY(EditAnywhere)
	//TSubclassOf<ASfCharacter> WeaponBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
