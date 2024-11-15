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
	
	const FVector MovementDirection = Character->GetCharacterMovement()->Velocity.GetSafeNormal();

    if(Character->PlayerType == TEnumAsByte<TypeOfPlayer>::EnumType::Knight)
    {
    	Character->SetCanBeDamaged(false);	    
    }

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(Character->DashDistance));
	
	Character->LaunchCharacter(MovementDirection * Character->DashDistance, true, true);

	//Peut a changer plus tard, rajouter un timer si besoin en fonction des intéractions qu'il y aura	
	StateMachine->ChangeState(ESfCharacterStateID::Walk);
}

void USfCharacterDash::StateExit(ESfCharacterStateID NextStateID)
{
	if(Character->PlayerType == TEnumAsByte<TypeOfPlayer>::EnumType::Knight)
	{
		Character->SetCanBeDamaged(true);	    
	}
}

void USfCharacterDash::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);
}
