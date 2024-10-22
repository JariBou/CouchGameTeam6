	// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SfCharacterStateID.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TypeOfPickable.h"
#include "SfCharacter.generated.h"

	class USfCharacterInputData;
class USfCharacterState;
class USfCharacterStateMachine;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class BP_PickableItem_C;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ASfCharacter : public ACharacter
{
	GENERATED_BODY()
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	
public:
	ASfCharacter();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<TypeOfPlayer> PlayerType;
	
	UFUNCTION(BlueprintCallable, meta=(TypeOfPlayer))
	void ChangePlayerType(TEnumAsByte<TypeOfPlayer> TypeOfPlayer);
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

#pragma region Input Data / Mapping Context

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USfCharacterInputData> InputData;

protected:
	//void SetupMappingContextIntoController() const;
	
#pragma endregion

#pragma region Input Move X
	
public:
	FVector2D GetInputMove() const;

protected:
	UPROPERTY()
	FVector2D InputMove = {0.f, 0.f};

private:
	void OnInputMove(const FInputActionValue& InputActionValue);
	
	void BindInputMoveAndActions(UEnhancedInputComponent* EnhancedInputComponent);
	
#pragma endregion 

#pragma region StateMachine

public:
	void CreateStateMachine();

	void InitStateMachine();

	void TickStateMachine(float DeltaTime) const;

	TMap<ESfCharacterStateID, TSubclassOf<USfCharacterState>> GetPossibleStates();

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USfCharacterStateMachine> StateMachine;
	
	UPROPERTY(EditAnywhere)
	TMap<ESfCharacterStateID, TSubclassOf<USfCharacterState>> PossibleStates;
	
#pragma endregion
};
