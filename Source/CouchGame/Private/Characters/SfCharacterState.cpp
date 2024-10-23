// Fill out your copyright notice in the Description page of Project Settings.


#include "SfCharacterState.h"

#include "Characters/SfCharacter.h"
#include "Characters/SfCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

USfCharacterState::USfCharacterState()
{
}

ESfCharacterStateID USfCharacterState::GetStateID() const
{
	return ESfCharacterStateID::None;
}

void USfCharacterState::StateInit(USfCharacterStateMachine* InStateMachine)
{
	StateMachine = InStateMachine;
	Character = InStateMachine->GetCharacter();
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Magenta, FString::Printf(TEXT("Init State %d"), GetStateID()));
}

void USfCharacterState::StateEnter(ESfCharacterStateID PreviousStateID)
{
	Character->PlayAnimMontage(Montage);

	PlayerMaxWalkSpeedOnEnter = Character->GetCharacterMovement()->MaxWalkSpeed;
}

void USfCharacterState::StateExit(ESfCharacterStateID NextStateID)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = PlayerMaxWalkSpeedOnEnter;
}

void USfCharacterState::StateTick(float DeltaTime)
{
}
