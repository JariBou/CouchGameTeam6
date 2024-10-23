// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SfCharacterState.h"
#include "SfCharacterDash.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class COUCHGAME_API USfCharacterDash : public USfCharacterState
{
	GENERATED_BODY()

public:
	virtual ESfCharacterStateID GetStateID() const override;

	virtual void StateInit(USfCharacterStateMachine* InStateMachine) override;
	
	virtual void StateEnter(ESfCharacterStateID PreviousStateID) override;
	
	virtual void StateExit(ESfCharacterStateID NextStateID) override;
	
	virtual void StateTick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float DashDistance = 500.f;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
};
