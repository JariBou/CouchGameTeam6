// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerMenu.generated.h"

struct FInputActionValue;
class UMenuInputData;
/**
 * 
 */
UCLASS()
class ROYALRIOT_API APlayerControllerMenu : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay();

	virtual void SetupInputComponent() override;


#pragma region Input Data / Mapping Context

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMenuInputData> InputData;

protected:
	UPROPERTY()
	UEnhancedInputComponent* EnhancedInputComponent;

private :
	void SetInputData(UMenuInputData* NewInputData);

	void BindInputActions();

	void OnNavigate(const FInputActionValue& InputActionValue);

	void OnConfirm(const FInputActionValue& InputActionValue);

	void OnBack(const FInputActionValue& InputActionValue);
	
#pragma endregion
};
