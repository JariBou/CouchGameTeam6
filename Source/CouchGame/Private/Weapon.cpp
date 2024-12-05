// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"


// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Cast<UPrimitiveComponent>(RootComponent)->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshComponent->SetSimulatePhysics(true);
	FWeaponInfo* row = Weapon.DataTable->FindRow<FWeaponInfo>(Weapon.RowName, "");
	if (row == nullptr)
	{
		Destroy();
		return;
	}
	SetCurrentData(*row);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Holder == nullptr)
	{
		Life -= DeltaTime;
		if (Life < 0)
		{
			Life = 0;
			Destroy();
		}
	}

	m_speed = (GetActorLocation() - m_lastFramePos).Length();
	m_lastFramePos = GetActorLocation();

	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::SanitizeFloat(GetDamage()));
}

float AWeapon::GetDamage() const
{
	if (Holder == nullptr) return 0.0f;
	if (Holder->PlayerType == Squire) return 0;
	return CurrentDataRow.WeaponStats.Damage * m_speed * CurrentDataRow.WeaponStats.DMGMultiplier;
}

void AWeapon::DealtDamage()
{
	if (Holder == nullptr) return;

	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Weapon Takes DAMAAAAAAAAGE");
	Durability--;
	if (Durability <= 0)
	{
		Holder->Drop();
		Destroy();
	}
}

FWeaponInfo& AWeapon::GetDataWeaponRowInfo(FName NameOfRow)
{
	return *Weapon.DataTable->FindRow<FWeaponInfo>(NameOfRow, "");
}

void AWeapon::SetCurrentData(FWeaponInfo NewData)
{
	CurrentDataRow = NewData;
	Durability = CurrentDataRow.WeaponStats.Durability;
	Life = CurrentDataRow.WeaponStats.Lifetime;
	//StaticMeshComponent->SetStaticMesh(CurrentDataRow.WeaponMesh.LoadSynchronous());
}

