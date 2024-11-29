	// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "SfCharacterStateID.h"
#include "Teams.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TypeOfPickable.h"
#include "Camera/CameraFollowTarget.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "SfCharacter.generated.h"

	class UBoxComponent;
class APickable;
struct FInputActionInstance;
//struct FPhysicalAnimationData;
class UPhysicalAnimationComponent;
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
class ASfCharacter : public ACharacter, public ICameraFollowTarget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

	UFUNCTION()
	void OnDelegateStickCircleLate();


#pragma region CameraFollowTarget
public:
	virtual FVector GetFollowTarget() override;
	
	virtual bool IsFollowable() override;

#pragma endregion

private:

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

	UPROPERTY()
	bool IsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float InputRightJoystickDeadZone = 0.5f;
	
	FVector2D InputRJ = FVector2d(0.f,0.f);

	float CurrentAngle; //Current Yaw Rotation Of Actor
	float DestinationAngle; //Destination Rotation Based On RightJoystick

public:
	/** Constructeur */
	ASfCharacter();

	/**Player Type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<TypeOfPlayer> PlayerType = Squire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETeam> PlayerTeam;
	
	/**Change Player Type */
	UFUNCTION(BlueprintCallable, meta=(TypeOfPlayer))
	void ChangePlayerType(TEnumAsByte<TypeOfPlayer> TypeOfPlayer);
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

private:
	void SetInputData(USfCharacterInputData* NewInputData);
	
#pragma endregion

#pragma region Input Move X
	
public:
	FVector2D GetInputMove() const;

	void StartDashCooldownTimer();

	bool CanDash = true;

protected:
	UPROPERTY()
	FVector2D InputMove = {0.f, 0.f};

	UPROPERTY(EditAnywhere)
	float DashCooldown = 0.f;

	float DashCooldownTimer = 0.f;

private:
	void OnInputMove(const FInputActionValue& InputActionValue);

	void OnInputRun(const FInputActionValue& InputActionValue);

	void OnInputDash(const FInputActionValue& InputActionValue);

	void RightJoystickInput(const FInputActionValue& InputActionValue);

	void RightJoystickStarted(const FInputActionValue& InputActionValue);

	void RightJoystickEnded(const FInputActionValue& InputActionValue);
	
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

private:
	void SetPossibleStates(TMap<ESfCharacterStateID, TSubclassOf<USfCharacterState>> NewPossibleStates);
	
#pragma endregion

#pragma region Arms Ragdoll

public:
	void SetUpArmsRagdoll() ;
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicalAnimationComponent> PhysicalComponent;

	UPROPERTY(EditAnywhere)
	FName BoneNameToApplyRagdoll;

	/*UPROPERTY()
	FTransform BoneTransformToApplyRagdoll;

	UPROPERTY(EditAnywhere)
	FName BoneNameToMove;
 
	UPROPERTY()
	FTransform BoneTransformToMove;*/

	UPROPERTY(EditAnywhere)
	FPhysicalAnimationData PhysicalAnimationData;
	
#pragma endregion

#pragma region Health

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthValueChange, class ASfCharacter*, CallingCharacter);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health;

	UPROPERTY(EditAnywhere)
	uint8 NumberOfTimeHealthIsUsed = 0;
	//ASfCharacter CallingCharacter = this;
	
public:
	UPROPERTY(BlueprintAssignable, Category="Event")
	FOnHealthValueChange OnHealthValueChange;
	
	UFUNCTION(BlueprintCallable)
	void TakeDamageCustom(ASfCharacter* DmgDealer, float Amount);

	UFUNCTION()
	void AddHealth(float HealthToAdd);

	UFUNCTION()
	void ChangeSkeletalMesh(USkeletalMesh* SkeletalMesh) const;

	UFUNCTION()
	void SetupHealth(uint8 inMaxHealth);
	
#pragma endregion

#pragma region PickUpAndThrow
protected:
	UFUNCTION()
	void PickUpAndThrowAction(const FInputActionInstance& Instance);

	AActor* GetClosestActorToCharacterInArray(TArray<AActor*>& ArrayOfPickable) const;
	
	UFUNCTION()
	void PickUpAndThrow(TArray<AActor*>& ArrayOfPickable);

	// void OnPickableCollisionTimeout();
	
	void OnPickableCollisionTimeout(APickable* Pickable);

	APickable* Drop(); //Drop Object

	void PickupObject(APickable* Pickable); //Give Object TO Player = THIS

	void GiveToKnight();

	void Interact();

public:
	UPROPERTY(BlueprintReadWrite)
	bool IsCarrying = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category= "Pickable")
	TObjectPtr<APickable> CurrentPickable;

	UPROPERTY()
	TObjectPtr<APickable> LastPickable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Pickable")
	TObjectPtr<UBoxComponent> CollisionForObject;

	// UPROPERTY()
	// TArray<AActor*> ListOfActorFromCollision;
	
	FTimerHandle TimerHandle;
	FTimerHandle TimerHandleForCircle;

	UPROPERTY(EditAnywhere, Category= "Pickable")
	float TimerForObjectCollisionWithPlayer = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Pickable")
	FString RightHandBoneName;

	/*
	UPROPERTY()
	TObjectPtr<AWell> WellInRange;
	*/

	UPROPERTY()
	TObjectPtr<ASfCharacter> FriendlyKnight;
			

#pragma endregion 

#pragma region InMud

public:
	UPROPERTY(EditAnywhere)
	float DashDistance = 0.f;

	void StartFeedBackEffect(bool IsLooping);

	void StopFeedBackEffect();

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UForceFeedbackEffect> ForceFeedbackEffect;

	UPROPERTY(EditAnywhere)
	FName ForceFeedBackEffectTag;
	
#pragma endregion

#pragma region CharacterRotation
private:
	void ManageCharacterRotation(float DeltaSeconds);

	float CurrentDeltaMadeByStick = 0.f;
	UPROPERTY(VisibleAnywhere)
	int NumberOfRotationMadeByStick = 0;

public:
	UPROPERTY(EditAnywhere)
	int RotationSpeed = 1.f;
	
#pragma endregion 
};

