// Copyright Epic Games, Inc. All Rights Reserved.

#include "CouchGame/Public/Characters/SfCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Pickable.h"
#include "Camera/CameraWorldSubsystem.h"
#include "Characters/CharacterSettings.h"
#include "Characters/SfCharacterInputData.h"
#include "Characters/SfCharacterStateMachine.h"
#include "Components/BoxComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameplayElements/Events/VisualEventHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Modes/SfGameMode.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACouchGameCharacter

FVector ASfCharacter::GetFollowTarget()
{
	return GetActorLocation();
}

bool ASfCharacter::IsFollowable()
{
	return Health > 0;
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
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	//Create Sphere Coll For Object Detection
	CollisionForObject = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionForObject->SetupAttachment(RootComponent);

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
	
		

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AfterSuper"));

	Health = MaxHealth;
	
	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->AddFollowTarget(this);
	//Add Input Mapping Context
	// if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	// {
	// 	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	// 	{
	// 		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	// 	}
	// }
}

void ASfCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (IsCarrying) Drop();

	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->RemoveFollowTarget(this);
}

void ASfCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (StateMachine) StateMachine->Tick(DeltaSeconds);	
	
	//Clamping Z location between ZLocation of bone where we apply ragdoll and its own ZLocation
	//float ClampedZLocation = FMath::Clamp(BoneTransformToMove.GetLocation().Z, BoneTransformToApplyRagdoll.GetLocation().Z, BoneTransformToMove.GetLocation().Z);
	//Create new vector Location
	//FVector NewClampedLocation = FVector(BoneTransformToMove.GetLocation().X, BoneTransformToMove.GetLocation().Y, ClampedZLocation);
	//Set new Location
	//BoneTransformToMove.SetLocation(NewClampedLocation);
	//Set Bone transform with modifications

	if(DashCooldownTimer > 0.f && !CanDash)
	{
		DashCooldownTimer -= DeltaSeconds;
		
		if(DashCooldownTimer <= 0.f)
		{
			CanDash = true;
		}
	}
	if(CanDash)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Yellow, TEXT("TRUE"), false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Yellow, TEXT("FALSE"), false);
	}

	GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Yellow, FString::SanitizeFloat(DashCooldownTimer), false);

}

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
}

void ASfCharacter::OnInputRun(const FInputActionValue& InputActionValue)
{
	StateMachine->SetWantsToRun(InputActionValue.Get<bool>() ? !StateMachine->GetWantsToRun() : StateMachine->GetWantsToRun());
}

void ASfCharacter::OnInputDash(const FInputActionValue& InputActionValue)
{
	/*GEngine->AddOnScreenDebugMessage(
		-1,
		4.0f,
		FColor::Yellow,
		TEXT("OnInputDash"));*/
		
	
	StateMachine->ChangeState(ESfCharacterStateID::Dash);
}

void ASfCharacter::BindInputMoveAndActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (InputData == nullptr) return;

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
}

void ASfCharacter::StartDashCooldownTimer()
{
	CanDash = false;
	DashCooldownTimer = DashCooldown;
}

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
}

TMap<ESfCharacterStateID, TSubclassOf<USfCharacterState>> ASfCharacter::GetPossibleStates()
{
	return PossibleStates;
}

void ASfCharacter::SetPossibleStates(TMap<ESfCharacterStateID, TSubclassOf<USfCharacterState>> NewPossibleStates)
{
	PossibleStates = NewPossibleStates;
}

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

void ASfCharacter::TakeDamageCustom(ASfCharacter* DmgDealer, float Amount)
{
	if(CanBeDamaged())
	{
		Health -= Amount;
		OnHealthValueChange.Broadcast(this);
	}
	
	if (Health <= 0 && !IsDead)
	{
		IsDead = true;
		ASfGameMode* SfGameMode = Cast<ASfGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (SfGameMode != nullptr) SfGameMode->NotifyPlayerKilled(DmgDealer, this);
	}
}

void ASfCharacter::AddHealth(float HealthToAdd)
{
	Health += HealthToAdd;
	++NumberOfTimeHealthIsUsed; //Hurm actually c'est plus opti
}

void ASfCharacter::ChangeSkeletalMesh(USkeletalMesh* SkeletalMesh) const
{
	GetMesh()->SetAnimationMode(EAnimationMode::Type::AnimationCustomMode);
	GetMesh()->SetSkeletalMesh(SkeletalMesh);

	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		GetMesh()->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
	}));
}

void ASfCharacter::SetupHealth(uint8 inMaxHealth)
{
	MaxHealth = inMaxHealth;
	Health = MaxHealth;
}

void ASfCharacter::PickUpAndThrowAction(const FInputActionInstance& Instance)
{
	TArray<AActor*> ListOfActorFromCollision;
	//Btw si j'avais dit de créer un BP du puits c'est pas pour rien....
	CollisionForObject->GetOverlappingActors(ListOfActorFromCollision, UVisualEventHandler::StaticClass());
	PickUpAndThrow();

	// if(ListOfActorFromCollision.IsEmpty())
	// {
	// }
	// else
	// {
	// 	for (AActor* Well : ListOfActorFromCollision)
	// 	{
	// 		//Cast<UWell>(Well)
	// 		//Do My Shit
	// 	}
	// }
}

AActor* ASfCharacter::GetClosestActorToCharacterInArray(TArray<AActor*>& ArrayOfPickable)
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

void ASfCharacter::PickUpAndThrow()
{
	GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Emerald, TEXT("PICK UP DE FOU CA MARCHE STP"));

	if(IsCarrying) //Si il porte un objet
	{
		Drop();
	} else //Si il n'en a pas dans les mains
	{
		TArray<AActor*> ArrayOfOverlappingObjects;
		CollisionForObject->GetOverlappingActors(ArrayOfOverlappingObjects, APickable::StaticClass());
		CurrentPickable = Cast<APickable>(GetClosestActorToCharacterInArray(ArrayOfOverlappingObjects));
		
		if(CurrentPickable != nullptr)
		{
			if(CurrentPickable->Implements<UInteractions>()) //Si il contient l'interface
			{
				if(CurrentPickable->CanPickUp_Implementation(this))
				{
					CurrentPickable->Holder = this;
					CurrentPickable->StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
					CurrentPickable->StaticMeshComponent->SetSimulatePhysics(false);
					const FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,EAttachmentRule::KeepRelative, true);
					CurrentPickable->AttachToComponent(this->GetMesh(),TransformRules,FName("hr"));
					IsCarrying = true;
				}
			}
		}
	}
}

void ASfCharacter::OnPickableCollisionTimeout()
{
	if(LastPickable != nullptr)	LastPickable->StaticMeshComponent->IgnoreActorWhenMoving(this, false);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void ASfCharacter::Drop()
{
#pragma region DetachPickable
		
	const FDetachmentTransformRules DeTransformRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, true);
	CurrentPickable->DetachFromActor(DeTransformRules);
	IsCarrying = false;
		
#pragma endregion

#pragma region Get Vel And Impulse

	FVector CurrentVelocity = FVector(this->GetCharacterMovement()->GetLastUpdateVelocity().X,this->GetCharacterMovement()->GetLastUpdateVelocity().Y,0.f);
	if(CurrentVelocity.Length() > 0.f) //Velocity supérieur a 0
	{
		//Get Impulse

		FVector ImpulseDirection = FVector(this->GetActorForwardVector().X * 500.f, this->GetActorForwardVector().Y * 500.f, 1.f * 200.f); //IMPULSE DIRECTION (NO GD FRIENDLY)
		ImpulseDirection += this->GetVelocity();
		CurrentPickable->StaticMeshComponent->AddImpulse(ImpulseDirection, FName(""), true); //IMPULSE
	}
	CurrentPickable->StaticMeshComponent->SetSimulatePhysics(true);
	CurrentPickable->StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);

	CurrentPickable->StaticMeshComponent->IgnoreActorWhenMoving(this, true);

	//Timer Delegate
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &ASfCharacter::OnPickableCollisionTimeout);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, TimerForObjectCollisionWithPlayer, false);
	LastPickable = CurrentPickable;
	CurrentPickable = nullptr;
#pragma endregion 
}

void ASfCharacter::StartFeedBackEffect(bool IsLooping)
{
	FForceFeedbackParameters FeedbackParams;
	FeedbackParams.bLooping = IsLooping;
	FeedbackParams.Tag = ForceFeedBackEffectTag;
	
	Cast<APlayerController>(GetController())->ClientPlayForceFeedback(ForceFeedbackEffect, FeedbackParams);
}

void ASfCharacter::StopFeedBackEffect()
{
	Cast<APlayerController>(GetController())->ClientStopForceFeedback(ForceFeedbackEffect, ForceFeedBackEffectTag);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASfCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent == nullptr) return;

	SetInputData(GetDefault<UCharacterSettings>()->GetInputDataFromPlayerType(PlayerType));
	SetPossibleStates(InputData->CharacterStates);
	
	BindInputMoveAndActions(EnhancedInputComponent);

}


void ASfCharacter::ChangePlayerType(TEnumAsByte<TypeOfPlayer> TypeOfPlayer)
{
	if (PlayerType == TypeOfPlayer) return;
	PlayerType = TypeOfPlayer;
	if (TypeOfPlayer == Knight)
	{
		if (IsCarrying) Drop();
	}
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
