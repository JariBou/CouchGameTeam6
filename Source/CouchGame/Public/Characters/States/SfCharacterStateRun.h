// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SfCharacterState.h"
#include "SfCharacterStateRun.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class COUCHGAME_API USfCharacterStateRun : public USfCharacterState
{
	GENERATED_BODY()

public:
	virtual ESfCharacterStateID GetStateID() const override;

	virtual void StateInit(USfCharacterStateMachine* InStateMachine) override;

	virtual void StateEnter(ESfCharacterStateID PreviousStateID) override;

	virtual void StateExit(ESfCharacterStateID NextStateID) override;

	virtual void StateTick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	float MaxRunSpeed = 1000.f;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
};
