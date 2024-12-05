// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickable.h"
#include "WeaponsDataAsset.h"
#include "Weapon.generated.h"

UCLASS()
class COUCHGAME_API AWeapon : public APickable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	private:
	UPROPERTY(VisibleAnywhere)
	float Life = 10;
	

#pragma region DamageLogic

public:
	UFUNCTION(BlueprintCallable)
	float GetDamage() const;

	UFUNCTION(BlueprintCallable)
	void DealtDamage();

	UFUNCTION(BlueprintCallable)
	bool DealDamage(ASfCharacter* Target);
	
private:
	UPROPERTY()
	FVector m_lastFramePos;

	UPROPERTY()
	float m_speed;

	UPROPERTY()
	bool JustDealtDamage;
	
#pragma endregion

#pragma region Data Table
public:
	#pragma region Property
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType = "WeaponInfo"))
	FDataTableRowHandle Weapon;

	UPROPERTY(BlueprintReadWrite)
	FWeaponInfo CurrentDataRow;

	UPROPERTY(BlueprintReadWrite)
	int Speed;

	UPROPERTY(BlueprintReadWrite)
	uint8 Durability = 10;

	#pragma endregion 

	UFUNCTION(BlueprintCallable)
	FWeaponInfo& GetDataWeaponRowInfo(FName NameOfRow);

	UFUNCTION(BlueprintCallable)
	void SetCurrentData(FWeaponInfo NewData);
#pragma endregion



#pragma region Sounds

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> WeaponHitSound;
	
#pragma endregion 
};
