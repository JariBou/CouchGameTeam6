// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SfCharacterStateIdle.h"

#include "Characters/SfCharacter.h"
#include "Characters/SfCharacterStateMachine.h"

ESfCharacterStateID USfCharacterStateIdle::GetStateID() const
{
	return ESfCharacterStateID::Idle;
}

void USfCharacterStateIdle::StateEnter(ESfCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Blue,
		TEXT("ENTER StateIdle")
	);
}

void USfCharacterStateIdle::StateExit(ESfCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("EXIT StateIdle")
	);
}

void USfCharacterStateIdle::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Green,
		TEXT("TICK StateIdle")
	);

	if(Character->GetVelocity().Length() > 0.1f)
	{
		StateMachine->ChangeState(ESfCharacterStateID::Walk);
	}
}
