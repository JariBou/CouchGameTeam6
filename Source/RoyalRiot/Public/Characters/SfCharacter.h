	// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <UI/IndicatorWidget.h>

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "NiagaraComponent.h"
#include "SfCharacterStateID.h"
#include "Teams.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TypeOfPickable.h"
#include "Camera/CameraFollowTarget.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "SfCharacter.generated.h"

	class UWidgetComponent;
	class UNiagaraSystem;
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
	
	UPROPERTY(EditAnywhere, Category="Materials For Characters")
	UMaterialInterface* MaterialKTeam1;

	UPROPERTY(EditAnywhere, Category="Materials For Characters")
	UMaterialInterface* MaterialKTeam2;

	UPROPERTY(EditAnywhere, Category="Materials For Characters")
	UMaterialInterface* MaterialSTeam1;

	UPROPERTY(EditAnywhere, Category="Materials For Characters")
	UMaterialInterface* MaterialSTeam2;

	//Only Niagara Component, used only when niagara decide to exist (je te hais niagara)
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> NiagaraComponentOfPlayer;
	UPROPERTY(EditAnywhere, Category="VFX")
	TObjectPtr<UNiagaraSystem> NSHealth;

	UFUNCTION()
	void OnDelegateStickCircleLate();

	UFUNCTION()
	void OnDelegateStickCircleThrustEnd();


#pragma region CameraFollowTarget
public:
	virtual FVector GetFollowTarget() override;
	
	virtual bool IsFollowable() override;

	#pragma endregion
	
#pragma region Misc
private:
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float InputRightJoystickDeadZone = 0.5f;
	
	FVector2D InputRJ = FVector2d(0.f,0.f);

	float CurrentAngle; //Current Yaw Rotation Of Actor
	float DestinationAngle; //Destination Rotation Based On RightJoystick

	UFUNCTION()
	void NiagaraSpawn(UNiagaraSystem* NSToUse, USceneComponent* ComponentToAttach);

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
	void ChangePlayerType(TEnumAsByte<TypeOfPlayer> TypeOfPlayer, bool ForceUpdate = false);

	

	
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
#pragma endregion

#pragma region Input Data / Mapping Context

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USfCharacterInputData> InputData;

protected:
	//void SetupMappingContextIntoController() const;
	UPROPERTY()
	UEnhancedInputComponent* EnhancedInputComponent;

private:
	void SetInputData(USfCharacterInputData* NewInputData);
	
#pragma endregion

#pragma region Input Move X
	
public:
	FVector2D GetInputMove() const;

	UFUNCTION()
	void StartDashCooldownTimer();

	UPROPERTY()
	bool CanDash = true;

protected:
	UPROPERTY()
	FVector2D InputMove = {0.f, 0.f};

	UPROPERTY(EditAnywhere)
	float DashCooldown = 0.f;

	UPROPERTY()
	float DashCooldownTimer = 0.f;

private:
	void OnInputMove(const FInputActionValue& InputActionValue);

	void OnInputRun(const FInputActionValue& InputActionValue);

	void OnInputDash(const FInputActionValue& InputActionValue);

	void RightJoystickInput(const FInputActionValue& InputActionValue);
	
	void RightJoystickStarted(const FInputActionValue& InputActionValue);

	void RightJoystickEnded(const FInputActionValue& InputActionValue);
	
	void BindInputMoveAndActions();
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicalAnimationComponent* PhysicalAnimationComponent;
	
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

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ActivateRagdollArms(bool State);

	
#pragma endregion

#pragma region Health
protected:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthValueChange, class ASfCharacter*, CallingCharacter);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health;

	UPROPERTY(EditAnywhere)
	uint8 NumberOfTimeHealthIsUsed = 0;
	//ASfCharacter CallingCharacter = this;

	UPROPERTY()
	bool IsUnderInvincibilityTime;

	float InvisibilityAdvancement = 0.f;
	

public:
	UPROPERTY(BlueprintReadWrite)
	bool IsDead = false;

	UPROPERTY(EditAnywhere, Category= "DmgIndicator")
	float TimeForDmgVisual = 1.0f;
	bool IsMatDmgRed = false;
	float DmgRedAdvancement = 0.f;
	
	UPROPERTY(BlueprintAssignable, Category="Event")
	FOnHealthValueChange OnHealthValueChange;
	
	UFUNCTION(BlueprintCallable)
	bool CanBeDamagedCustom();
	
	UFUNCTION(BlueprintCallable)
	bool TakeDamageCustom(ASfCharacter* DmgDealer, float Amount);

	UFUNCTION(BlueprintCallable)
	void Kill(ASfCharacter* DmgDealer);

	UFUNCTION()
	void RemoveInvincibility();

	UFUNCTION()
	void AddHealth(float HealthDelta, bool IsVisual);

	UFUNCTION()
	void UsedHealingSource();
	
	UFUNCTION()
	void ChangeSkeletalMesh(USkeletalMesh* SkeletalMesh);

	UFUNCTION()
	void SetupHealth(uint8 inMaxHealth);

	void SetInvincibility(bool bCond);

	
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
	
	public:
	APickable* Drop(); //Drop Object
	
	protected:
	void PickupObject(APickable* Pickable, bool Force = false); //Give Object TO Player = THIS

	void GiveToKnight(ASfCharacter* FriendlyKnight);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Hurtbox")
	TObjectPtr<UBoxComponent> CollisionForPlayer;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UUserWidget> FeedbackWidget;

	UFUNCTION(BlueprintCallable)
	void SetFeedbackWidget(UUserWidget* NewFeedbackWidget);

	// UPROPERTY()
	// TArray<AActor*> ListOfActorFromCollision;
	
	FTimerHandle TimerHandle;
	FTimerHandle TimerHandleForCircle;
	FTimerHandle TimerHandleForThrust;

	UPROPERTY(EditAnywhere, Category= "Pickable")
	float TimerForObjectCollisionWithPlayer = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Pickable")
	FString RightHandBoneName;

	/*
	UPROPERTY()
	TObjectPtr<AWell> WellInRange;
	*/
			

#pragma endregion 

#pragma region InMud

public:
	UPROPERTY(EditAnywhere)
	float DashDistance = 0.f;
	
#pragma endregion

#pragma region CharacterRotation
private:
	void ManageCharacterRotation(float DeltaSeconds);

	float CurrentDeltaMadeByStick = 0.f;
	
	UPROPERTY(VisibleAnywhere, Category="Rotation")
	int NumberOfRotationMadeByStick = 0;

public:
	UPROPERTY(EditAnywhere, Category="Rotation")
	int RotationSpeed = 1.f;

	UPROPERTY(EditAnywhere, Category="Rotation")
	int NumberOfRotationNeeded = 1;

	UPROPERTY(EditAnywhere, Category="Rotation")
	float TimeNeededForRotation = 1.f;

	UPROPERTY(EditAnywhere, Category="Rotation")
	float TimeNeedForThrust = 0.25f;

	UPROPERTY(EditAnywhere, Category="Rotation")
	int MaxAngleForThrust = 10;
	
#pragma endregion
	
#pragma region Sounds

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTriggerSound);

public:
	UPROPERTY(BlueprintAssignable, Category="event")
	FTriggerSound TriggerDropSound;
	
	UPROPERTY(BlueprintAssignable, Category="event")
	FTriggerSound TriggerDeathSound;

	UPROPERTY(BlueprintAssignable, Category="event")
	FTriggerSound TriggerMoveSound;

	UPROPERTY(BlueprintAssignable, Category="event")
	FTriggerSound TriggerDodgeSound;

	UPROPERTY(BlueprintAssignable, Category="event")
	FTriggerSound TriggerPickupSound;

	UPROPERTY(BlueprintAssignable, Category="event")
	FTriggerSound TriggerTakeDamageSound;

	UPROPERTY(BlueprintAssignable, Category="event")
	FTriggerSound TriggerRespawnSound;
	
	//Ce code est pas fou Clément, j'aurais pas fait comme ca à ta place, pas fifou, Change le ce serait mieux
	
	/*UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> KnightDropSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> SquireDropSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> KnightDeathSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> SquireDeathSound;*/

	
	
#pragma endregion 

#pragma region Animations & display

	public:
	UPROPERTY(BlueprintReadOnly)
	FVector DirectionForAnimVector;

	UPROPERTY(BlueprintReadOnly)
	bool IsRotating;

	UPROPERTY()
	bool IsRotationAnimLaunched = false;

	UPROPERTY()
	bool IsThrustAnimLaunched = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PlumComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> RotationAnimMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> RotationAnimMontageRevert;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> LungeAnimMontage;
	
	UFUNCTION()
	void OnAnimMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* IndixatorWidgetComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* PcWidgetComponent;

	UPROPERTY()
	UIndicatorWidget* DashIndicator;
	
	UPROPERTY()
	UIndicatorWidget* PcIndicator;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<TypeOfPlayer>, float> ControllerDisplayZOffset;

	UFUNCTION()
	void UpdateControllerIdDisplay(int ControllerId);

	#pragma endregion

#pragma region Vibrations feedbacks

public:
	void StartFeedBackEffect(UForceFeedbackEffect* ForceFeedbackEffect, FName ForceFeedBackEffectTag, bool IsLooping);

	void StopFeedBackEffect(UForceFeedbackEffect* ForceFeedbackEffect, FName ForceFeedbackEffectTag);
	
#pragma endregion
	
};

