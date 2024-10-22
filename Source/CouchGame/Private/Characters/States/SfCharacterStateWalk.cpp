// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SfCharacterStateWalk.h"

#include "Characters/SfCharacter.h"
#include "Characters/SfCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

ESfCharacterStateID USfCharacterStateWalk::GetStateID() const
{
	return ESfCharacterStateID::Walk;
}

void USfCharacterStateWalk::StateInit(USfCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);
	MovementComponent = Character->FindComponentByClass<UCharacterMovementComponent>();
}

void USfCharacterStateWalk::StateEnter(ESfCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	Character->GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("ENTER StateWalk")
	);
}

void USfCharacterStateWalk::StateExit(ESfCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("EXIT StateWalk")
	);
}

void USfCharacterStateWalk::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("TICK StateWalk")
	);

	if(Character->GetVelocity().Length() < 0.1f)
	{
		StateMachine->ChangeState(ESfCharacterStateID::Idle);
	}
	else
	{
		FVector2D InputMove = Character->GetInputMove();
		Character->AddMovementInput(FVector(InputMove.X, InputMove.Y, 0));
	}
}
