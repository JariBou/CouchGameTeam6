// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SfCharacterDash.h"

#include "Characters/SfCharacter.h"
#include "Characters/SfCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

ESfCharacterStateID USfCharacterDash::GetStateID() const
{
	return ESfCharacterStateID::Dash;
}

void USfCharacterDash::StateInit(USfCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);
	MovementComponent = Character->FindComponentByClass<UCharacterMovementComponent>();
}

void USfCharacterDash::StateEnter(ESfCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	
	const FVector MovementDirection = Character->GetActorRotation().Vector();

	//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Emerald, MovementDirection.ToString());
	
	Character->LaunchCharacter(MovementDirection * DashDistance, true, true);


	//Peut a changer plus tard, rajouter un timer si besoin en fonction des intéractions qu'il y aura	
	StateMachine->ChangeState(ESfCharacterStateID::Walk);
}

void USfCharacterDash::StateExit(ESfCharacterStateID NextStateID)
{

}

void USfCharacterDash::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);
}
