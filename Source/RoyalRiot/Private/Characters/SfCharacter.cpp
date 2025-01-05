// Copyright Epic Games, Inc. All Rights Reserved.

#include "RoyalRiot/Public/Characters/SfCharacter.h"

#include <Chaos/PBDNullConstraints.h>
#include <Components/WidgetComponent.h>
#include <Consumables/Consumable.h>
#include <UI/IndicatorWidget.h>

#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
#include "Pickable.h"
#include "Camera/CameraWorldSubsystem.h"
#include "Characters/CharacterSettings.h"
#include "Characters/SfCharacterInputData.h"
#include "Characters/SfCharacterStateMachine.h"
#include "Characters/VibrationsFeedBack/Vibrations.h"
#include "Components/BoxComponent.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "GameplayElements/WaterBucket.h"
#include "GameplayElements/Events/VisualEventHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/SfGameMode.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACouchGameCharacter

void ASfCharacter::OnDelegateStickCircleLate()
{
	if(FMath::Abs(NumberOfRotationMadeByStick) >= NumberOfRotationNeeded && !IsThrustAnimLaunched && !IsRotationAnimLaunched &&!IsDashing)
	{
		// Réussite du stick toupie lol
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Réussi"));
		// TODO play anim montage
		IsRotationAnimLaunched = true;
		int Sign = FMath::Sign(NumberOfRotationMadeByStick);
		ActivateRagdollArms(false);
		if (Sign == 0) Sign = 1;
		if(Sign >= 0)
		{
			PlayAnimMontage(RotationAnimMontageRevert, RotationAnimMontageRevert->RateScale);
		} else
		{
			PlayAnimMontage(RotationAnimMontage, RotationAnimMontage->RateScale);
		}
	}
	// GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, TEXT("Fin de Stick Delay"));
	CurrentDeltaMadeByStick = 0.f;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

#pragma region AActor & Interfaces

FVector ASfCharacter::GetFollowTarget()
{
	return GetActorLocation();
}

bool ASfCharacter::IsFollowable()
{
	return Health > 0;
}

void ASfCharacter::NiagaraSpawn(UNiagaraSystem* NSToUse, USceneComponent* ComponentToAttach)
{
	NiagaraComponentOfPlayer = UNiagaraFunctionLibrary::SpawnSystemAttached(
	NSToUse,
	ComponentToAttach,
	NAME_None,
	FVector(0.f,0.f,0.f),
	FRotator(0.f),
	EAttachLocation::Type::SnapToTarget,
	true);
	if(IsValid(NiagaraComponentOfPlayer))	NiagaraComponentOfPlayer->SetUsingAbsoluteRotation(true);
}

ASfCharacter::ASfCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	// CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// CameraBoom->SetupAttachment(RootComponent);
	// CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	// CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	//Create Sphere Coll For Object Detection
	CollisionForObject = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionForObject->SetupAttachment(RootComponent);
	
	CollisionForPlayer = CreateDefaultSubobject<UBoxComponent>(TEXT("Hurtbox"));
	CollisionForPlayer->SetupAttachment(RootComponent);

	PlumComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlumComponent"));
	PlumComponent->SetupAttachment(GetMesh(), "Head");

	IndixatorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DashIndicator"));
	IndixatorWidgetComponent->SetupAttachment(RootComponent);
	
	PcWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerControllerIndicator"));
	PcWidgetComponent->SetupAttachment(RootComponent);

	PhysicalAnimationComponent = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("PhysicalAnimationComponent"));

	// Create a follow camera
/*
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
*/
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASfCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	CreateStateMachine();
	InitStateMachine();
	//SetUpArmsRagdoll();
	

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AfterSuper"));
	
	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->AddFollowTarget(this);
	
	const UCharacterSettings* CharacterSettings = GetDefault<UCharacterSettings>();

	// MAKES NO FUCKING SENSE BUT WE NEED THSI HERE
	SetupHealth(CharacterSettings->CharacterInputDatas[PlayerType].MaxHealth);
	SetActorScale3D(CharacterSettings->CharacterInputDatas[PlayerType].Scale);


	CurrentAngle = GetActorRotation().Yaw;
	DestinationAngle = CurrentAngle;
	// G pas les môts
	// FVector Forward = GetActorForwardVector().RotateAngleAxis(CurrentAngle, FVector::UpVector);
	// FVector Forward = FVector(1, 0, 0).RotateAngleAxis(CurrentAngle, FVector::UpVector);
	// InputRJ = FVector2d(Forward.X, Forward.Y);
	// InputRJ = FVector2d(GetActorForwardVector().X, GetActorForwardVector().Y);
	InputRJ = FVector2d(1, 0);

	GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &ASfCharacter::OnAnimMontageNotify);

	// ActivateRagdollArms();

	DashIndicator = Cast<UIndicatorWidget>(IndixatorWidgetComponent->GetWidget());
	if (DashIndicator) DashIndicator->HideIndicator();

	PcIndicator = Cast<UIndicatorWidget>(PcWidgetComponent->GetWidget());
	// if (PcIndicator) PcIndicator->InitWithValue(UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(GetController())));
	UpdateControllerIdDisplay(UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(GetController())));
	
	FVector PcWidgetComponentRelativeLocation = PcWidgetComponent->GetRelativeLocation();
	PcWidgetComponent->SetRelativeLocation(FVector(PcWidgetComponentRelativeLocation.X, PcWidgetComponentRelativeLocation.Y, ControllerDisplayZPosition[PlayerType]));
	
	FVector IndixatorWidgetComponentRelativeLocation = IndixatorWidgetComponent->GetRelativeLocation();
	IndixatorWidgetComponent->SetRelativeLocation(FVector(IndixatorWidgetComponentRelativeLocation.X, IndixatorWidgetComponentRelativeLocation.Y, IndixatorWidgetComponentRelativeLocation.Z + DashIndicatorZOffset[PlayerType]));

	CanDash = true;
	DashCooldownTimer = 0;
}

void ASfCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (IsCarrying) CurrentPickable->Destroy();
	
	GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ASfCharacter::OnAnimMontageNotify);

	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->RemoveFollowTarget(this);
}

void ASfCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (StateMachine) StateMachine->Tick(DeltaSeconds);

	//Dmg Visual Effect
	if(IsMatDmgRed)
	{
		DmgRedAdvancement += DeltaSeconds / TimeForDmgVisual;
		GetMesh()->SetScalarParameterValueOnMaterials("HitValue", FMath::Clamp(FMath::Sin(DmgRedAdvancement * PI),0.f , 1.f));
		if(DmgRedAdvancement >= 1.f)
		{
			IsMatDmgRed = false;
		}
	}

	if(IsUnderInvincibilityTime)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("Dissolve", 0.f); // Necessarily for Fader To Work
		InvisibilityAdvancement += DeltaSeconds;
		float InvisibilityVfxValue = FMath::Clamp(1.f - FMath::Abs(FMath::Sin(InvisibilityAdvancement * PI * 2)),0.5f , 1.f);
		GetMesh()->SetScalarParameterValueOnMaterials("Fader", InvisibilityVfxValue);
	}

	

	ManageCharacterRotation(DeltaSeconds);

	if(DashCooldownTimer > 0.f && !CanDash)
	{
		if(IndixatorWidgetComponent && DashIndicator)
		{
			DashIndicator->UpdateValue(FMath::Clamp(1- DashCooldownTimer/DashCooldown, 0.f, 1.f));
		}
		DashCooldownTimer -= DeltaSeconds;
		
		if(DashCooldownTimer <= 0.f)
		{
			CanDash = true;
			if (IndixatorWidgetComponent && DashIndicator) DashIndicator->HideIndicator();
		}
	}
	if(CanDash)
	{
		// GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Yellow, TEXT("TRUE"), false);
	}
	else
	{
		// GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Yellow, TEXT("FALSE"), false);
	}

	#pragma region Anim

	FVector2D ActorForwardVector = FVector2D(GetActorForwardVector().X, GetActorForwardVector().Y);
	ActorForwardVector.Normalize();

	FVector2D InputMoveSnap = InputMove;
	InputMoveSnap.Normalize();

	FVector ActorRightVector = GetActorForwardVector().RotateAngleAxis(90, FVector::UpVector);
	FVector2D RightVector = FVector2D(ActorRightVector.X, ActorRightVector.Y);
	float AngleSign = FMath::Sign(FVector2D::DotProduct(RightVector, InputMoveSnap));
	float DotProduct = FVector2D::DotProduct(ActorForwardVector, InputMoveSnap);
	float AngleInRadians = FMath::Acos(DotProduct);
	float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians) * -AngleSign;

	FVector DirectionVector = FVector(1, 0, 0).RotateAngleAxis(AngleInDegrees, FVector::UpVector);

	DirectionForAnimVector = DirectionVector * InputMove.Length();

	#pragma endregion

	// GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Yellow, FString::SanitizeFloat(DashCooldownTimer), false);

}

void ASfCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent == nullptr) return;

	SetInputData(GetDefault<UCharacterSettings>()->GetInputDataFromPlayerType(PlayerType));
	SetPossibleStates(InputData->CharacterStates);
	
	BindInputMoveAndActions();

}

#pragma endregion

#pragma region Movement

void ASfCharacter::SetInputData(USfCharacterInputData* NewInputData)
{
	InputData = NewInputData;
}

FVector2D ASfCharacter::GetInputMove() const
{
	return InputMove;
}

void ASfCharacter::OnInputMove(const FInputActionValue& InputActionValue)
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Magenta, FString::Printf(TEXT("AGUGUGAGA")));

	InputMove = InputActionValue.Get<FVector2D>();

	TriggerMoveSound.Broadcast();
}

void ASfCharacter::OnInputRun(const FInputActionValue& InputActionValue)
{
	StateMachine->SetWantsToRun(InputActionValue.Get<bool>() ? !StateMachine->GetWantsToRun() : StateMachine->GetWantsToRun());
}

void ASfCharacter::OnInputDash(const FInputActionValue& InputActionValue)
{
	//TriggerDodgeSound.Broadcast();
	if (!CanDash || IsThrustAnimLaunched || IsRotationAnimLaunched) return;
	// Pas ouf de changer de state dans tout les cas
	if (DashIndicator) DashIndicator->ShowIndicator();
	StateMachine->ChangeState(ESfCharacterStateID::Dash);
	if(IsValid(NSTrace))
	{
		NiagaraSpawn(NSTrace, GetMesh());
		NiagaraComponentOfPlayer->SetWorldLocation(GetMesh()->GetBoneLocation("Spine03"));
	}
}

void ASfCharacter::RightJoystickInput(const FInputActionValue& InputActionValue)
{
	FVector2d TempInputRJValue = InputActionValue.Get<FVector2D>(); //Case of Stick Length >= Dead Zone
	if(TempInputRJValue.SquaredLength() > InputRightJoystickDeadZone * InputRightJoystickDeadZone)
	{
		TempInputRJValue = InputRJ;
		InputRJ = InputActionValue.Get<FVector2D>();
		
		//float DeltaAngle = FMath::Atan2(InputRJ.Y, InputRJ.X) - FMath::Atan2(TempInputRJValue.Y, TempInputRJValue.X);

		float DeltaAngle = FMath::Atan2(InputRJ.Y*TempInputRJValue.X - InputRJ.X*TempInputRJValue.Y, InputRJ.X*TempInputRJValue.X + InputRJ.Y*TempInputRJValue.Y);

		CurrentDeltaMadeByStick += DeltaAngle;
		NumberOfRotationMadeByStick = static_cast<int>(CurrentDeltaMadeByStick / (2 * PI));
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Emerald, FString::FromInt(NumberOfRotationMadeByStick));
		
		DestinationAngle += DeltaAngle;
	}
}

void ASfCharacter::OnDelegateStickCircleThrustEnd()
{
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::SanitizeFloat(FMath::RadiansToDegrees(CurrentDeltaMadeByStick)));
	// if(FMath::Abs(FMath::RadiansToDegrees(CurrentDeltaMadeByStick)) >= MaxAngleForThrust) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Stick Superior"));

	// TODO: should only be called on joystick cancelation basically, Or should it?
	if (!IsThrustAnimLaunched && !IsRotationAnimLaunched && !IsDashing)
	{
		ActivateRagdollArms(false);
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("StickThrustEnd"));
		PlayAnimMontage(LungeAnimMontage);
	}
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleForThrust);
}

void ASfCharacter::RightJoystickStarted(const FInputActionValue& InputActionValue)
{
	CurrentDeltaMadeByStick = 0.f;
	// IsRotationAnimLaunched = false; //TO CHANGE IN ANIM 
	FTimerDelegate TimerDelegateForStickCircleCount;
	FTimerDelegate TimerDelegateForStickCirlceThrust;
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Début Stick"));
	TimerDelegateForStickCircleCount.BindUObject<ASfCharacter>(this, &ASfCharacter::OnDelegateStickCircleLate);
	TimerDelegateForStickCirlceThrust.BindUObject<ASfCharacter>(this, &ASfCharacter::OnDelegateStickCircleThrustEnd);
	GetWorld()->GetTimerManager().SetTimer(TimerHandleForCircle, TimerDelegateForStickCircleCount, TimeNeededForRotation, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandleForThrust, TimerDelegateForStickCirlceThrust, TimeNeedForThrust, false);
}

void ASfCharacter::RightJoystickEnded(const FInputActionValue& InputActionValue)
{
	if(FMath::Abs(NumberOfRotationMadeByStick) >= NumberOfRotationNeeded && !IsRotationAnimLaunched && !IsThrustAnimLaunched && !IsDashing)
	{
		// Réussite du stick toupie lol
		// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Réussi"));
		IsRotationAnimLaunched = true;
		int Sign = FMath::Sign(NumberOfRotationMadeByStick);
		ActivateRagdollArms(false);
		if (Sign == 0) Sign = 1;
		if(Sign >= 0)
		{
			PlayAnimMontage(RotationAnimMontageRevert, RotationAnimMontageRevert->RateScale);

		} else
		{
			PlayAnimMontage(RotationAnimMontage, RotationAnimMontage->RateScale);
		}
	}
	CurrentDeltaMadeByStick = 0.f;
	// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Ended"));
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleForCircle);
}

void ASfCharacter::BindInputMoveAndActions()
{
	if (InputData == nullptr) return;
	if (EnhancedInputComponent == nullptr) return;
	EnhancedInputComponent->ClearActionBindings();

	if(InputData->InputActionLeftJoystick) //Move
	{
		EnhancedInputComponent->BindAction(
			InputData->InputActionLeftJoystick,
			ETriggerEvent::Started,
			this,
			&ASfCharacter::OnInputMove);

		EnhancedInputComponent->BindAction(
			InputData->InputActionLeftJoystick,
			ETriggerEvent::Completed,
			this,
			&ASfCharacter::OnInputMove);

		EnhancedInputComponent->BindAction(
			InputData->InputActionLeftJoystick,
			ETriggerEvent::Triggered,
			this,
			&ASfCharacter::OnInputMove);
	}

	if(InputData->InputActionLeftJoystickButton) // Run
	{
		EnhancedInputComponent->BindAction(
			InputData->InputActionLeftJoystickButton,
			ETriggerEvent::Started,
			this,
			&ASfCharacter::OnInputRun);

		EnhancedInputComponent->BindAction(
			InputData->InputActionLeftJoystickButton,
			ETriggerEvent::Completed,
			this,
			&ASfCharacter::OnInputRun);
	}

	if(InputData->InputActionFaceButtonDown) // Squire : Dash,   Knight : Dodge
	{
		EnhancedInputComponent->BindAction(
		InputData->InputActionFaceButtonDown,
		ETriggerEvent::Started,
		this,
		&ASfCharacter::OnInputDash);

		EnhancedInputComponent->BindAction(
			InputData->InputActionFaceButtonDown,
			ETriggerEvent::Completed,
			this,
			&ASfCharacter::OnInputDash);
	
	}

	if(InputData->InputActionFaceButtonUp) // Squire : Interact
	{
		
	}

	if(InputData->InputActionFaceButtonRight) // Squire : Interact
	{
		
	}

	if(InputData->InputActionFaceButtonLeft) // Squire : Take, Give, Throw,      Knight : Take, Throw
	{
		EnhancedInputComponent->BindAction(InputData->InputActionFaceButtonLeft,
			ETriggerEvent::Started,
			this,
			&ASfCharacter::PickUpAndThrowAction);
	}

	if(InputData->InputActionRightTrigger) // Squire : Taunt
	{
		//
	}

	if(InputData->InputActionLeftTrigger) // Squire : Slap
	{
		//Je slap tes grosses fessiers bien rondes et dodus et soyeuses et rambombés et galbées et rebondis
	}

	if(InputData->InputActionRightJoystick)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionRightJoystick,
		ETriggerEvent::Triggered,
		this,
		&ASfCharacter::RightJoystickInput);

		EnhancedInputComponent->BindAction(InputData->InputActionRightJoystick,
		ETriggerEvent::Started,
		this,
		&ASfCharacter::RightJoystickStarted);

		EnhancedInputComponent->BindAction(InputData->InputActionRightJoystick,
		ETriggerEvent::Completed,
		this,
		&ASfCharacter::RightJoystickEnded);

		EnhancedInputComponent->BindAction(InputData->InputActionRightJoystick,
		ETriggerEvent::Canceled,
		this,
		&ASfCharacter::RightJoystickEnded);
	}
}

void ASfCharacter::StartDashCooldownTimer()
{
	CanDash = false;
	DashCooldownTimer = DashCooldown;
}

void ASfCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASfCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASfCharacter::ManageCharacterRotation(float DeltaSeconds)
{
	FRotator DestinationRotator = GetActorRotation();
	DestinationRotator.Yaw = FMath::RadiansToDegrees(DestinationAngle);
	//SetActorRotation(UKismetMathLibrary::RLerp(GetActorRotation(), DestinationRotator, DeltaSeconds * RotationSpeed, true), ETeleportType::TeleportPhysics);
	FRotator NewActorRotator = GetActorRotation();
	if (PlayerType == Knight)
	{
		if (IsRotationAnimLaunched) return;
		CurrentAngle = FMath::Lerp(CurrentAngle, DestinationAngle, DeltaSeconds * RotationSpeed);
		float ActorConvertedAngle = FMath::RadiansToDegrees(CurrentAngle) + 90.f;
		NewActorRotator = GetActorRotation();
		NewActorRotator.Yaw = ActorConvertedAngle;
	}else if (InputMove.Length() > .1f)
	{
		NewActorRotator = FVector(InputMove.X, InputMove.Y, 0).Rotation();
	}

	SetActorRotation(NewActorRotator, ETeleportType::TeleportPhysics);
}

#pragma endregion

#pragma region StateMachine

void ASfCharacter::CreateStateMachine()
{
	StateMachine = NewObject<USfCharacterStateMachine>(this);
}

void ASfCharacter::InitStateMachine()
{
	if (StateMachine == nullptr) return;
	StateMachine->Init(this);
}

void ASfCharacter::TickStateMachine(float DeltaTime) const
{
	if (StateMachine == nullptr) return;
	StateMachine->Tick(DeltaTime);

	// GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red, "Bouuuh, éteint ton PC la prochaine fois Clément Kiss Kiss");
}

TMap<ESfCharacterStateID, TSubclassOf<USfCharacterState>> ASfCharacter::GetPossibleStates()
{
	return PossibleStates;
}

void ASfCharacter::SetPossibleStates(TMap<ESfCharacterStateID, TSubclassOf<USfCharacterState>> NewPossibleStates)
{
	PossibleStates = NewPossibleStates;
}

#pragma endregion

void ASfCharacter::SetUpArmsRagdoll()
{
	if (PhysicalComponent!=nullptr) return;

	UPhysicalAnimationComponent* NewComp = NewObject<UPhysicalAnimationComponent>(this);
	NewComp->RegisterComponent();
	PhysicalComponent = NewComp;
	AddInstanceComponent(PhysicalComponent);

	PhysicalComponent->SetSkeletalMeshComponent(GetMesh());
	PhysicalComponent->ApplyPhysicalAnimationSettingsBelow(BoneNameToApplyRagdoll, PhysicalAnimationData, true);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(BoneNameToApplyRagdoll, true, false);

	//BoneTransformToApplyRagdoll = GetMesh()->GetBoneTransform(BoneNameToApplyRagdoll);
	//BoneTransformToMove = GetMesh()->GetBoneTransform(BoneNameToMove);
	
	//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Turquoise, BoneTransformToMove.ToHumanReadableString());
}

#pragma region Health And stuff

bool ASfCharacter::CanBeDamagedCustom()
{
	// Actually I'm not sure we really use this CanBeDamaged() but well, it works
	return CanBeDamaged() && !IsUnderInvincibilityTime && !IsDead;
}

bool ASfCharacter::TakeDamageCustom(ASfCharacter* DmgDealer, float Amount)
{
	if(CanBeDamagedCustom())
	{
		//Start vibrations when taking damages
		StartFeedBackEffect(
			VibrationsData->TakeDamage.ForceFeedbackEffect,
			VibrationsData->TakeDamage.Tag,
			false
		);	
		
		IsUnderInvincibilityTime = true;
		InvisibilityAdvancement = 0.f;
		TriggerTakeDamageSound.Broadcast();

		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, "Player Takes Damage");

		IsMatDmgRed = true;
		DmgRedAdvancement = 0.f;
		
		//Health -= Amount;
		AddHealth(-Amount, false);
		OnHealthValueChange.Broadcast(this);

		const UCharacterSettings* Settings = GetDefault<UCharacterSettings>();
		if (DmgDealer != nullptr)
		{
			FVector Direction = GetActorLocation() - DmgDealer->GetActorLocation();
			Direction.Normalize();
			Direction *= Settings->CharacterInputDatas[PlayerType].KnockbackForce;
			LaunchCharacter(Direction, false, false);
		}

		FTimerHandle NullHandle;
		GetGameInstance()->GetTimerManager().SetTimer(NullHandle, this, &ASfCharacter::RemoveInvincibility, Settings->CharacterInputDatas[PlayerType].InvincibilityTimeAfterHit);
		if (Health <= 0 && !IsDead)
		{
			Kill(DmgDealer);
		}
		return true;
	}
	
	return false;
}

void ASfCharacter::Kill(ASfCharacter* DmgDealer)
{
	IsDead = true;
	
	TriggerDeathSound.Broadcast();

	//Start vibrations when getting killed
	StartFeedBackEffect(
		VibrationsData->Death.ForceFeedbackEffect,
		VibrationsData->Death.Tag,
		false
		);

	//Start vibrations when killing opponent
	StartFeedBackEffect(
		VibrationsData->KillOpponent.ForceFeedbackEffect,
		VibrationsData->KillOpponent.Tag,
		false
	);
	
	
	ASfGameMode* SfGameMode = Cast<ASfGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (SfGameMode != nullptr) SfGameMode->NotifyPlayerKilled(DmgDealer, this);
}

void ASfCharacter::RemoveInvincibility()
{
	if (this == nullptr) return; // in case we have some sort of timer bug
	IsUnderInvincibilityTime = false;
	//Reset Dissolve Mask
	GetMesh()->SetScalarParameterValueOnMaterials("Dissolve", 10.f);
	GetMesh()->SetScalarParameterValueOnMaterials("Fader", 1.f);
}

void ASfCharacter::AddHealth(float HealthDelta, bool IsVisual)
{
	Health += HealthDelta;
	Health = FMath::Clamp(Health, -1.f, MaxHealth);
	OnHealthValueChange.Broadcast(this);
	if(IsVisual && IsValid(NSHealth)) NiagaraSpawn(NSHealth, PlumComponent); 
}

void ASfCharacter::UsedHealingSource()
{
	++NumberOfTimeHealthIsUsed;
}

void ASfCharacter::SetupHealth(uint8 inMaxHealth)
{
	MaxHealth = inMaxHealth;
	Health = MaxHealth;
}

void ASfCharacter::SetInvincibility(bool bCond)
{
	IsUnderInvincibilityTime = bCond;
}

#pragma endregion

// Change some skeletal mesh
void ASfCharacter::ChangeSkeletalMesh(USkeletalMesh* SkeletalMesh)
{
	// GetMesh()->SetAnimationMode(EAnimationMode::Type::AnimationSingleNode);
	// GetMesh()->SetAnimClass(nullptr);
	GetMesh()->SetSkeletalMesh(SkeletalMesh);
	if (PlayerType == Knight)
	{
		GetWorldTimerManager().SetTimerForNextTick([&]
		{
			ActivateRagdollArms(true);
			GetMesh()->SetAnimClass(GetDefault<UCharacterSettings>()->CharacterInputDatas[PlayerType].AnimBlueprint);
		});
	}
	
	// GetMesh()->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);

	UMaterialInstanceDynamic* DynMatTeam1;
	UMaterialInstanceDynamic* DynMatTeam2;
	
	if(PlayerType == TypeOfPlayer::Knight)
	{
		DynMatTeam1 = UMaterialInstanceDynamic::Create(MaterialKTeam1, nullptr);
		DynMatTeam2 = UMaterialInstanceDynamic::Create(MaterialKTeam2, nullptr);
	} else
	{
		DynMatTeam1 = UMaterialInstanceDynamic::Create(MaterialSTeam1, nullptr);
		DynMatTeam2 = UMaterialInstanceDynamic::Create(MaterialSTeam2, nullptr);
	}
	// For the moment
	//DynMat->SetVectorParameterValue("ColorParam", FColor::Green);
	//DynMat2->SetVectorParameterValue("ColorParam", FColor::Purple);
	if(PlayerTeam == ETeam::Team1 && IsValid(DynMatTeam1)) GetMesh()->SetMaterial(0, DynMatTeam1);
	if(PlayerTeam == ETeam::Team2 && IsValid(DynMatTeam2)) GetMesh()->SetMaterial(0, DynMatTeam2);
}

#pragma region Pickup & Give

/// Binded to the input
void ASfCharacter::PickUpAndThrowAction(const FInputActionInstance& Instance)
{	
	
	//Btw si j'avais dit de créer un BP du puits c'est pas pour rien....
	//C reel ca, mais va y c la faute de clément chef
	// if (IsDashing || IsDead) return;

	TArray<AActor*> ListOfActorFromCollision;
	UEventHandler* FoundWell = nullptr;
	//CHECK OBJ
	CollisionForObject->GetOverlappingActors(ListOfActorFromCollision, AActor::StaticClass()); // La Faute de clem ptn
	ListOfActorFromCollision.RemoveAll([&](const AActor* Actor)
	{
		return Actor == this || (CurrentPickable != nullptr && Actor == CurrentPickable);
	});
	//Setup FriendlyKnight && Well PAS OPTI
	ASfCharacter* FriendlyKnight = nullptr;
	
	for (AActor* ActorFromCollision : ListOfActorFromCollision)
	{
		// Warning: does not take into account if it's a friendly character or self
		if(ASfCharacter* Character = Cast<ASfCharacter>(ActorFromCollision);
			Character != nullptr && Character->PlayerTeam == PlayerTeam && Character->PlayerType == Knight)
		{
			FriendlyKnight = Character;
		}
		else if(UEventHandler* Well = Cast<UEventHandler>(ActorFromCollision->GetComponentByClass(UEventHandler::StaticClass())); Well != nullptr)
		{
			FoundWell = Well;
		}
		//else if(Cast<AWell>(ActorFromCollision) != nullptr) WellInRange = Cast<AWell>(ActorFromCollision);
	}
	
	AActor* ClosestActor = GetClosestActorToCharacterInArray(ListOfActorFromCollision);
	
	AWaterBucket* MyWaterBucket = Cast<AWaterBucket>(CurrentPickable);
	if(MyWaterBucket == nullptr && CurrentPickable != nullptr) //OUI JE LE SAIS TOMÉ JE LE FAIS DEJA APRES M'EN VEUX PAS STP
	{
		if(PlayerType == Squire && FriendlyKnight != nullptr)
		{
			// PlayAnimMontage(GiveObjectMontage);
			GiveToKnight(FriendlyKnight);
		}
		else if(APickable* obj = Cast<APickable>(ClosestActor); obj != nullptr) //Switch
		{
			//A checker car la on joue avec des pointeurs
			// To check 'cause I have no clue wtf is going on here
			// Drop() then Give()??
			Drop();
			PickupObject(obj);
		}
		else
		{
			Drop();
		}
	}
	else if(MyWaterBucket != nullptr)
	{
		if(FoundWell != nullptr && !MyWaterBucket->IsFilled)
		{
			MyWaterBucket->SwitchFillBucket();
		}
		else if(FoundWell == nullptr)
		{
			Drop();
		}
	}
	else
	{
		//PUAT
		PickUpAndThrow(ListOfActorFromCollision);
	}
}

AActor* ASfCharacter::GetClosestActorToCharacterInArray(TArray<AActor*>& ArrayOfPickable) const
{
	float MinDistance = FLT_MAX;
	AActor* ClosestPickable = nullptr;
	float CurrentDistance = 0.f;
	for (AActor* Pickable : ArrayOfPickable)
	{
		CurrentDistance = (Pickable->GetActorLocation() - this->GetActorLocation()).SquaredLength();
		if(CurrentDistance < MinDistance)
		{
			MinDistance = CurrentDistance;
			ClosestPickable = Pickable;
		}
	}
	return ClosestPickable;
}

void ASfCharacter::PickUpAndThrow(TArray<AActor*>& ArrayOfPickable)
{
	// GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Emerald, TEXT("PICK UP DE FOU CA MARCHE STP"));

	if(IsCarrying) //Si il porte un objet
	{
		Drop();
	} else //Si il n'en a pas dans les mains
	{
		ArrayOfPickable.RemoveAll([&](const AActor* Actor)
		{
			const APickable* Pickable = Cast<APickable>(Actor);
			return Pickable != nullptr && Pickable->Holder != nullptr;
		});
		APickable* PickableObject = Cast<APickable>(GetClosestActorToCharacterInArray(ArrayOfPickable));
		if(PickableObject != nullptr) PickupObject(PickableObject);
	}
}

void ASfCharacter::OnPickableCollisionTimeout(APickable* Pickable)
{
	if(IsValid(Pickable))	Pickable->StaticMeshComponent->IgnoreActorWhenMoving(this, false);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

APickable* ASfCharacter::Drop()
{
	if (CurrentPickable == nullptr) return nullptr;
	
	//Play Drop sound
	TriggerDropSound.Broadcast();
	AWaterBucket* WaterBucket = Cast<AWaterBucket>(CurrentPickable);

	//Detach Pickable
	const FDetachmentTransformRules DeTransformRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, true);
	CurrentPickable->DetachFromActor(DeTransformRules);
	IsCarrying = false;
		
	//Get Vel and Impulse
	FVector CurrentVelocity = FVector(this->GetCharacterMovement()->GetLastUpdateVelocity().X,this->GetCharacterMovement()->GetLastUpdateVelocity().Y,0.f);
	CurrentPickable->StaticMeshComponent->SetSimulatePhysics(true);
	if(CurrentVelocity.Length() > 0.f) //Velocity supérieur a 0
	{
		//Get Impulse
		USkeletalMeshComponent* wow = this->GetMesh();
		FVector ImpulseDirection = FVector(wow->GetForwardVector().X * 500.f, wow->GetForwardVector().Y * 500.f, 1.f * 200.f); //IMPULSE DIRECTION (NO GD FRIENDLY)
		ImpulseDirection += this->GetVelocity();
		CurrentPickable->StaticMeshComponent->AddImpulse(ImpulseDirection, FName(""), true); //IMPULSE

		if(WaterBucket != nullptr)
		{
			WaterBucket->TriggerThrowSound.Broadcast();
		}
	}
	CurrentPickable->StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	CurrentPickable->StaticMeshComponent->IgnoreActorWhenMoving(this, true);

	//Timer Delegate
	FTimerDelegate TimerDelegateForHandCollision;
	APickable* DroppedPickable = CurrentPickable;
	TimerDelegateForHandCollision.BindUObject<ASfCharacter>(this, &ASfCharacter::OnPickableCollisionTimeout, DroppedPickable);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegateForHandCollision, TimerForObjectCollisionWithPlayer, false);
	// LastPickable = DroppedPickable;
	CurrentPickable->Holder = nullptr;
	CurrentPickable = nullptr;
	return DroppedPickable;
}

void ASfCharacter::PickupObject(APickable* Pickable, bool Force)
{
	if(Pickable != nullptr)
	{
		// APickable inherits from IInteractions (~=UIntereactions) so shouldn't be needed to check if implements
		// Leaving it for now since it is 1:34am and I have no clue how to test that
		if(Pickable->Implements<UInteractions>()) //Si il contient l'interface
		{
			if(Force || Pickable->CanPickUp_Implementation(this)) //Peut prendre selon son role
			{
				TriggerPickupSound.Broadcast();
				// Pickable->Holder = this;
				Pickable->Interact_Implementation(this);
				// Pickable->NiagaraDropSystem_Implementation();
				Pickable->StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
				Pickable->StaticMeshComponent->SetSimulatePhysics(false);
				const FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,EAttachmentRule::KeepRelative, true);
				Pickable->AttachToComponent(this->GetMesh(),TransformRules,FName(RightHandBoneName));
				IsCarrying = true;
				CurrentPickable = Pickable;
				if (Pickable->FeedbackWidget)
				{
					Pickable->FeedbackWidget->RemoveFromParent();
					Pickable->FeedbackWidget = nullptr;
				}
				//PlayAnimMontage(PickupObjectMontage);
				AWaterBucket* WaterBucket = Cast<AWaterBucket>(Pickable);
				if(WaterBucket != nullptr)
				{
					WaterBucket->TriggerPickupSound.Broadcast();
				}
			}
		}
	}
}

void ASfCharacter::GiveToKnight(ASfCharacter* FriendlyKnight)
{
	if(FriendlyKnight != nullptr)
	{
		FriendlyKnight->FeedbackWidget->RemoveFromParent();
		CurrentPickable->FeedbackWidget = nullptr;

		//Start vibrations on Squire when giving item
		StartFeedBackEffect(
						VibrationsData->SquireGive.ForceFeedbackEffect,
						VibrationsData->SquireGive.Tag,
						false
		);

		//Start vibrations on FriendlyKnight when receiving item
		FriendlyKnight->StartFeedBackEffect(
						VibrationsData->KnightReceive.ForceFeedbackEffect,
						VibrationsData->KnightReceive.Tag,
						false
		);
		
		APickable* DroppedPickable = Drop(); //Lache Son Arme
		
		if (AConsumable* Consumable = Cast<AConsumable>(DroppedPickable); Consumable != nullptr)
		{
			Consumable->GetConsumedBy(FriendlyKnight);
			return;
		}
		
		SetInvincibility(true);
		
		if (FriendlyKnight->CurrentPickable != nullptr) FriendlyKnight->Drop()->Destroy();

		TriggerDropSound.Broadcast();
		FriendlyKnight->PickupObject(DroppedPickable, true); //Met l'arme dans sa main
		
		
		const UCharacterSettings* Settings = GetDefault<UCharacterSettings>();
		FTimerHandle NullHandle;
		GetGameInstance()->GetTimerManager().SetTimer(NullHandle, this, &ASfCharacter::RemoveInvincibility, Settings->InvincibilityTimeAfterGive);
	}
}

void ASfCharacter::Interact()
{
	Drop();
}

void ASfCharacter::SetFeedbackWidget(UUserWidget* NewFeedbackWidget)
{
	if(FeedbackWidget == nullptr)
	{
		FeedbackWidget = NewFeedbackWidget;
	}
	else
	{
		FeedbackWidget->RemoveFromParent();
		FeedbackWidget = NewFeedbackWidget;
	}
}

#pragma endregion

void ASfCharacter::StartFeedBackEffect(UForceFeedbackEffect* ForceFeedbackEffect, FName ForceFeedBackEffectTag, bool IsLooping)
{
	FForceFeedbackParameters FeedbackParams;
	FeedbackParams.bLooping = IsLooping;
	FeedbackParams.Tag = ForceFeedBackEffectTag;
	
	Cast<APlayerController>(this->GetController())->ClientPlayForceFeedback(ForceFeedbackEffect, FeedbackParams);
}

void ASfCharacter::StopFeedBackEffect(UForceFeedbackEffect* ForceFeedbackEffect, FName ForceFeedbackEffectTag)
{
	Cast<APlayerController>(this->GetController())->ClientStopForceFeedback(ForceFeedbackEffect, ForceFeedbackEffectTag);
}

TObjectPtr<UVibrations> ASfCharacter::GetVibrationsData()
{
	return VibrationsData;
}

void ASfCharacter::OnAnimMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == "EndTourbilol")
	{
		IsRotationAnimLaunched = false;
		ActivateRagdollArms(true);
	} else if (NotifyName == "EndThrust")
	{
		IsThrustAnimLaunched = false;
		ActivateRagdollArms(true);
	} else if (NotifyName == "EndDash")
	{
		IsDashing = false;
		if (PlayerType == Knight) ActivateRagdollArms(true);
		//Je sais pas ce que je fais je comprends pas votre dash les gars
		if(IsValid(NiagaraComponentOfPlayer)) NiagaraComponentOfPlayer->Deactivate();
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("AnimMontage Notify"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, NotifyName.ToString());
}

void ASfCharacter::UpdateControllerIdDisplay(int ControllerId)
{
	if (PcIndicator) PcIndicator->InitWithValue(ControllerId);
}

void ASfCharacter::DoRagdoll()
{
	GetMesh()->SetSimulatePhysics(true);
	PcWidgetComponent->DestroyComponent();
	IndixatorWidgetComponent->DestroyComponent();
}

//////////////////////////////////////////////////////////////////////////
// Input
void ASfCharacter::ChangePlayerType(TEnumAsByte<TypeOfPlayer> TypeOfPlayer, bool ForceUpdate)
{
	if (!ForceUpdate && PlayerType == TypeOfPlayer) return; // No Bloody clue as to why this doesn't work as expected
	TEnumAsByte<::TypeOfPlayer> FormerPlayerType = PlayerType;
	PlayerType = TypeOfPlayer;
	const UCharacterSettings* Settings = GetDefault<UCharacterSettings>();
	FCharacterSettingsData CharacterSettingsData = Settings->CharacterInputDatas[TypeOfPlayer];

	SetActorScale3D(CharacterSettingsData.Scale);
	SetupHealth(CharacterSettingsData.MaxHealth);

	ChangeSkeletalMesh(CharacterSettingsData.Mesh.LoadSynchronous());
	
	if (TypeOfPlayer == Knight)
	{
		if (FormerPlayerType == Squire && IsCarrying) Drop();
		
		// ActivateRagdollArms();
		// GetWorldTimerManager().SetTimerForNextTick([&]
		// {
		// 	ActivateRagdollArms();
		// });
	}

	FVector PcWidgetComponentRelativeLocation = PcWidgetComponent->GetRelativeLocation();
	PcWidgetComponent->SetRelativeLocation(FVector(PcWidgetComponentRelativeLocation.X, PcWidgetComponentRelativeLocation.Y, ControllerDisplayZPosition[TypeOfPlayer]));
	
	InputData = Settings->GetInputDataFromPlayerType(PlayerType);
	BindInputMoveAndActions();
}


