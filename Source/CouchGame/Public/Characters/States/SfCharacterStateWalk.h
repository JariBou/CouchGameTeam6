// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SfCharacterState.h"
#include "SfCharacterStateWalk.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class COUCHGAME_API USfCharacterStateWalk : public USfCharacterState
{
public:
	virtual ESfCharacterStateID GetStateID() const override;

	virtual void StateInit(USfCharacterStateMachine* InStateMachine) override;
	
	virtual void StateEnter(ESfCharacterStateID PreviousStateID) override;
	
	virtual void StateExit(ESfCharacterStateID NextStateID) override;
	
	virtual void StateTick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float MaxSpeed = 500.f;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
	
private:
	GENERATED_BODY()
};
