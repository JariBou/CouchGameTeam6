// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SfCharacterStateID.h"
#include "UObject/Object.h"
#include "SfCharacterState.generated.h"

class ASfCharacter;
class USfCharacterStateMachine;
/**
 * 
 */
UCLASS(Abstract)
class COUCHGAME_API USfCharacterState : public UObject
{
	GENERATED_BODY()

public:
	USfCharacterState();

	virtual ESfCharacterStateID GetStateID() const;

	virtual void StateInit(USfCharacterStateMachine* InStateMachine);
	
	virtual void StateEnter(ESfCharacterStateID PreviousStateID);
	
	virtual void StateExit(ESfCharacterStateID NextStateID);

	virtual void StateTick(float DeltaTime);
	
protected:
	UPROPERTY()
	TObjectPtr<ASfCharacter> Character;
	
	UPROPERTY()
	TObjectPtr<USfCharacterStateMachine> StateMachine;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY()
	float PlayerMaxWalkSpeedOnEnter;
};
