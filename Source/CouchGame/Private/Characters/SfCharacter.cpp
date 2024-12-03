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
#include "GameplayElements/WaterBucket.h"
#include "GameplayElements/Events/VisualEventHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Modes/SfGameMode.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACouchGameCharacter

void ASfCharacter::OnDelegateStickCircleLate()
{
	if(FMath::Abs(NumberOfRotationMadeByStick) >= NumberOfRotationNeeded)
	{
		// Réussite du stick toupie lol
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Réussi"));
		IsRotationAnimLaunched = true;
	}
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, TEXT("Fin de Stick Delay"));
	CurrentDeltaMadeByStick = 0.f;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

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
	
	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->AddFollowTarget(this);
	
	const UCharacterSettings* CharacterSettings = GetDefault<UCharacterSettings>();

	SetupHealth(CharacterSettings->CharacterInputDatas[PlayerType].MaxHealth);

	CurrentAngle = GetActorRotation().Yaw;
	DestinationAngle = CurrentAngle;
	InputRJ = FVector2d(1.f,0.f);
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

	ManageCharacterRotation(DeltaSeconds);

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

	#pragma region Anim

	FVector2D ActorForwardVector = FVector2D(GetActorForwardVector().X, GetActorForwardVector().Y);
	ActorForwardVector.Normalize();

	FVector2D InputMoveSnap = InputMove;
	InputMoveSnap.Normalize();

	FVector ActorRightVector = GetActorForwardVector().RotateAngleAxis(90, FVector::UpVector);
	FVector2D RightVector = FVector2D(ActorRightVector.X, ActorRightVector.Y);
	float AngleSign = FMath::Sign(FVector2D::DotProduct(RightVector, InputMoveSnap));
	float DotProduct = FVector2D::DotProduct(ActorForwardVector, InputMoveSnap);
	float DotSign = FMath::Sign(DotProduct);
	float AngleInRadians = FMath::Acos(DotProduct);
	float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians) * -AngleSign;

	FVector DirectionVector = FVector(1, 0, 0).RotateAngleAxis(AngleInDegrees, FVector::UpVector);
	if (DotSign < 0)
	{
		//DirectionVector.Y = -DirectionVector.Y;
	}
	DirectionForAnimVector = DirectionVector;

	// FVector Intermediate = GetActorForwardVector() * InputMoveSnap.Length();
	// Intermediate.Normalize();
	// // FVector2D AngleVector = FVector2D(ActorForwardVector.X, ActorForwardVector.Y) - InputMove;
	// // double Angle = FMath::Atan2(AngleVector.Y, AngleVector.X);
	// DirectionForAnimVector = FVector(1, 0, 0) .RotateAngleAxis(AngleInDegrees, FVector::UpVector);

	GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Emerald, DirectionForAnimVector.ToString(), false);
	GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Emerald, FString::SanitizeFloat(AngleInDegrees), false);

	#pragma endregion

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
		NumberOfRotationMadeByStick = int(CurrentDeltaMadeByStick / (2 * PI));
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Emerald, FString::FromInt(NumberOfRotationMadeByStick));
		
		DestinationAngle += DeltaAngle;
	}
}

void ASfCharacter::OnDelegateStickCicleThrustEnd()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::SanitizeFloat(FMath::RadiansToDegrees(CurrentDeltaMadeByStick)));
	if(FMath::Abs(FMath::RadiansToDegrees(CurrentDeltaMadeByStick)) >= MaxAngleForThrust) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Stick Superior"));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("StickThrustEnd"));
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleForThrust);
}

void ASfCharacter::RightJoystickStarted(const FInputActionValue& InputActionValue)
{
	CurrentDeltaMadeByStick = 0.f;
	IsRotationAnimLaunched = false; //TO CHANGE IN ANIM 
	FTimerDelegate TimerDelegateForStickCircleCount;
	FTimerDelegate TimerDelegateForStickCirlceThrust;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Début Stick"));
	TimerDelegateForStickCircleCount.BindUObject<ASfCharacter>(this, &ASfCharacter::OnDelegateStickCircleLate);
	TimerDelegateForStickCirlceThrust.BindUObject<ASfCharacter>(this, &ASfCharacter::OnDelegateStickCicleThrustEnd);
	GetWorld()->GetTimerManager().SetTimer(TimerHandleForCircle, TimerDelegateForStickCircleCount, TimeNeededForRotation, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandleForThrust, TimerDelegateForStickCirlceThrust, TimeNeedForThrust, false);
}

void ASfCharacter::RightJoystickEnded(const FInputActionValue& InputActionValue)
{
	if(FMath::Abs(NumberOfRotationMadeByStick) >= NumberOfRotationNeeded && IsRotationAnimLaunched == false)
	{
		// Réussite du stick toupie lol
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Réussi"));
	}
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Ended"));
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleForCircle);
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

	if(InputData->InputActionRightJoystick)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionRightJoystick,
		ETriggerEvent::Triggered,
		this,
		&ASfCharacter::RightJoystickInput);
	}

	if(InputData->InputActionRightJoystick)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionRightJoystick,
		ETriggerEvent::Started,
		this,
		&ASfCharacter::RightJoystickStarted);
	}

	if(InputData->InputActionRightJoystick)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionRightJoystick,
		ETriggerEvent::Completed,
		this,
		&ASfCharacter::RightJoystickEnded);
	}

	if(InputData->InputActionRightJoystick)
	{
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

bool ASfCharacter::CanBeDamagedCustom()
{
	// Actually I'm not sure we really use this CanBeDamaged() but well, it works
	return CanBeDamaged() && !IsUnderInvincibilityTime;
}

void ASfCharacter::TakeDamageCustom(ASfCharacter* DmgDealer, float Amount)
{
	if(CanBeDamagedCustom())
	{
		Health -= Amount;
		OnHealthValueChange.Broadcast(this);
		IsUnderInvincibilityTime = true;

		const UCharacterSettings* Settings = GetDefault<UCharacterSettings>();
		if (DmgDealer != nullptr)
		{
			FVector Direction = GetActorLocation() - DmgDealer->GetActorLocation();
			Direction.Normalize();
			Direction *= Amount * Settings->CharacterInputDatas[PlayerType].ForcePerDmg;
			LaunchCharacter(Direction, false, false);
		}

		FTimerHandle NullHandle;
		GetGameInstance()->GetTimerManager().SetTimer(NullHandle, this, &ASfCharacter::RemoveInvincibility, Settings->CharacterInputDatas[PlayerType].InvincibilityTime);
	}
	
	if (Health <= 0 && !IsDead)
	{
		IsDead = true;

		if(PlayerType == TEnumAsByte<TypeOfPlayer>::EnumType::Knight)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), KnightDeathSound,GetActorLocation());
		if(PlayerType == TEnumAsByte<TypeOfPlayer>::EnumType::Squire)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SquireDeathSound,GetActorLocation());
		
		ASfGameMode* SfGameMode = Cast<ASfGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (SfGameMode != nullptr) SfGameMode->NotifyPlayerKilled(DmgDealer, this);
	}
}

void ASfCharacter::RemoveInvincibility()
{
	IsUnderInvincibilityTime = false;
}

void ASfCharacter::AddHealth(float HealthToAdd)
{
	Health += HealthToAdd;
	++NumberOfTimeHealthIsUsed; //Hurm actually c'est plus opti
}

void ASfCharacter::ChangeSkeletalMesh(USkeletalMesh* SkeletalMesh) const
{
	// GetMesh()->SetAnimationMode(EAnimationMode::Type::AnimationSingleNode);
	GetMesh()->SetSkeletalMesh(SkeletalMesh);
	GetMesh()->SetAnimClass(GetDefault<UCharacterSettings>()->CharacterInputDatas[PlayerType].AnimBlueprint);
	// GetMesh()->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
	UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(Material, nullptr);
	UMaterialInstanceDynamic* DynMat2 = UMaterialInstanceDynamic::Create(Material, nullptr);
	DynMat->SetVectorParameterValue("ColorParam", FColor::Green);
	DynMat2->SetVectorParameterValue("ColorParam", FColor::Purple);
	if(PlayerTeam == ETeam::Team1) GetMesh()->SetMaterial(0, DynMat);
	if(PlayerTeam == ETeam::Team2) GetMesh()->SetMaterial(0, DynMat2);
}

void ASfCharacter::SetupHealth(uint8 inMaxHealth)
{
	MaxHealth = inMaxHealth;
	Health = MaxHealth;
}

void ASfCharacter::PickUpAndThrowAction(const FInputActionInstance& Instance)
{
	//Btw si j'avais dit de créer un BP du puits c'est pas pour rien....
	//C reel ca, mais va y c la faute de clément chef

	TArray<AActor*> ListOfActorFromCollision;
	UEventHandler* FoundWell = nullptr;
	//CHECK OBJ
	CollisionForObject->GetOverlappingActors(ListOfActorFromCollision, AActor::StaticClass()); //La Faute de clem ptn
	ListOfActorFromCollision.RemoveAll([&](const AActor* Actor){return Actor == this;});
	//Setup FriendlyKnight && Well PAS OPTI
	for (AActor* ActorFromCollision : ListOfActorFromCollision)
	{
		// Warning: does not take into account if it's a friendly character or self
		if(ASfCharacter* Character = Cast<ASfCharacter>(ActorFromCollision); Character != nullptr)
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
		if(FriendlyKnight != nullptr)
		{
			GiveToKnight();
		}
		else if(APickable* obj = Cast<APickable>(ClosestActor); obj != nullptr) //Switch
		{
			//A checker car la on joue avec des pointeurs
			// To check 'cause I have no clue wtf is going on here
			// Drop() then Give()??
			Drop();
			PickupObject(obj);
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
	GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Emerald, TEXT("PICK UP DE FOU CA MARCHE STP"));

	if(IsCarrying) //Si il porte un objet
	{
		Drop();
	} else //Si il n'en a pas dans les mains
	{
		APickable* PickableObject = Cast<APickable>(GetClosestActorToCharacterInArray(ArrayOfPickable));
		if(PickableObject != nullptr) PickupObject(PickableObject);
	}
}

void ASfCharacter::OnPickableCollisionTimeout(APickable* Pickable)
{
	if(Pickable != nullptr)	Pickable->StaticMeshComponent->IgnoreActorWhenMoving(this, false);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

APickable* ASfCharacter::Drop()
{
	//Play Drop sound
	if(PlayerType == TypeOfPlayer::Knight)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), KnightDropSound, GetActorLocation());
	else if(PlayerType == TypeOfPlayer::Squire)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SquireDropSound, GetActorLocation());
	
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
	}
	CurrentPickable->StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	CurrentPickable->StaticMeshComponent->IgnoreActorWhenMoving(this, true);

	//Timer Delegate
	FTimerDelegate TimerDelegateForHandCollision;
	APickable* DroppedPickable = CurrentPickable;
	TimerDelegateForHandCollision.BindUObject<ASfCharacter>(this, &ASfCharacter::OnPickableCollisionTimeout, DroppedPickable);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegateForHandCollision, TimerForObjectCollisionWithPlayer, false);
	// LastPickable = DroppedPickable;
	CurrentPickable = nullptr;
	return DroppedPickable;
}

void ASfCharacter::PickupObject(APickable* Pickable)
{
	if(Pickable != nullptr)
	{
		// APickable inherits from IInteractions (~=UIntereactions) so shouldn't be needed to check if implements
		// Leaving it for now since it is 1:34am and I have no clue how to test that
		if(Pickable->Implements<UInteractions>()) //Si il contient l'interface
		{
			if(Pickable->CanPickUp_Implementation(this)) //Peut prendre selon son role
			{
				Pickable->Holder = this;
				Pickable->StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
				Pickable->StaticMeshComponent->SetSimulatePhysics(false);
				const FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,EAttachmentRule::KeepRelative, true);
				Pickable->AttachToComponent(this->GetMesh(),TransformRules,FName(RightHandBoneName));
				IsCarrying = true;
				CurrentPickable = Pickable;
			}
		}
	}
}

void ASfCharacter::GiveToKnight()
{
	if(FriendlyKnight != nullptr)
	{
		APickable* DroppedPickable = Drop(); //Lache Son Arme
		FriendlyKnight->PickupObject(DroppedPickable); //Met l'arme dans sa main
	}
}

void ASfCharacter::Interact()
{
	Drop();
	//Interaction Event sur Puit a coder
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

void ASfCharacter::ManageCharacterRotation(float DeltaSeconds)
{
	FRotator DestinationRotator = GetActorRotation();
	DestinationRotator.Yaw = FMath::RadiansToDegrees(DestinationAngle);
	//SetActorRotation(UKismetMathLibrary::RLerp(GetActorRotation(), DestinationRotator, DeltaSeconds * RotationSpeed, true), ETeleportType::TeleportPhysics);
	
	CurrentAngle = FMath::Lerp(CurrentAngle, DestinationAngle, DeltaSeconds * RotationSpeed);
	float ActorConvertedAngle = FMath::RadiansToDegrees(CurrentAngle) + 90.f;
	FRotator NewActorRotator = GetActorRotation();
	NewActorRotator.Yaw = ActorConvertedAngle;
	SetActorRotation(NewActorRotator, ETeleportType::TeleportPhysics);
	
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
		const UCharacterSettings* Settings = GetDefault<UCharacterSettings>();
		ChangeSkeletalMesh(Settings->CharacterInputDatas[TypeOfPlayer].Mesh.LoadSynchronous());
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
