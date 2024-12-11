// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Controllers/PlayerControllerMenu.h"

#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"

#include "EnhancedInputSubsystems.h"
#include "LocalMultiplayerSettings.h"
#include "LocalMultiplayerSubsystem.h"
#include "Characters/CharacterSettings.h"
#include "Characters/Controllers/MenuInputData.h"
#include "Kismet/GameplayStatics.h"


void APlayerControllerMenu::BeginPlay()
{
	Super::BeginPlay();	
}

void APlayerControllerMenu::SetupInputComponent()
{
	Super::SetupInputComponent();

	EnhancedInputComponent = Cast<UEnhancedInputComponent>(GetComponentByClass(UEnhancedInputComponent::StaticClass()));
	if(EnhancedInputComponent == nullptr) return;

	SetInputData(GetDefault<UCharacterSettings>()->GetMenuInputData());
	
	//BindInputActions();
}

void APlayerControllerMenu::SetInputData(UMenuInputData* NewInputData)
{
	InputData = NewInputData;
}

void APlayerControllerMenu::BindInputActions()
{
	if(InputData == nullptr) return;
	if(EnhancedInputComponent == nullptr) return;

	if(InputData->Navigate)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Navigate");
		
		EnhancedInputComponent->BindAction(
			InputData->Navigate,
			ETriggerEvent::Started,
			this,
			&APlayerControllerMenu::OnNavigate);
	}

	if(InputData->Confirm)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Confirm");
		
		EnhancedInputComponent->BindAction(
			InputData->Confirm,
			ETriggerEvent::Started,
			this,
			&APlayerControllerMenu::OnConfirm
		);
	}

	if(InputData->Back)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Back");
		
		EnhancedInputComponent->BindAction(
			InputData->Back,
			ETriggerEvent::Started,
			this,
			&APlayerControllerMenu::OnBack
		);
	}
}

void APlayerControllerMenu::OnNavigate(const FInputActionValue& InputActionValue)
{
}

void APlayerControllerMenu::OnConfirm(const FInputActionValue& InputActionValue)
{
}

void APlayerControllerMenu::OnBack(const FInputActionValue& InputActionValue)
{
}


