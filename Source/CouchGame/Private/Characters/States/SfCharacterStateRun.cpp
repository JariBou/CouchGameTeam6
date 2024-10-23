// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SfCharacterStateRun.h"

#include "Characters/SfCharacter.h"
#include "Characters/SfCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

ESfCharacterStateID USfCharacterStateRun::GetStateID() const
{
	return ESfCharacterStateID::Run;
}

void USfCharacterStateRun::StateInit(USfCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);
	MovementComponent = Character->FindComponentByClass<UCharacterMovementComponent>();
}

void USfCharacterStateRun::StateEnter(ESfCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	Character->GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Green,
		TEXT("ENTER StateRun"));
}

void USfCharacterStateRun::StateExit(ESfCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("EXIT StateRun"));
}

void USfCharacterStateRun::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	if (FMath::Abs(Character->GetInputMove().Length()) < 0.1f)
	{
		StateMachine->ChangeState(ESfCharacterStateID::Idle);
	}
	else
	{
		FVector2D InputMove = Character->GetInputMove();
		Character->AddMovementInput(FVector(InputMove.X, InputMove.Y, 0));

		if(!StateMachine->GetWantsToRun())
		{
			StateMachine->ChangeState(ESfCharacterStateID::Walk);
		}
	}
}
